#include "dns_hijack.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <string.h>
#include <stdint.h>

static const char *TAG = "cap_portal";

/* The SoftAP gateway IP that every A query resolves to. */
#define HIJACK_IP_ADDR "192.168.4.1"

/*
 * Minimal DNS response builder.
 *
 * DNS wire format (RFC 1035):
 *   Header (12 bytes): ID, flags, QDCOUNT, ANCOUNT, NSCOUNT, ARCOUNT
 *   Question section: QNAME (labels), QTYPE, QCLASS
 *   Answer section:   NAME (ptr), TYPE, CLASS, TTL, RDLENGTH, RDATA
 *
 * We copy the question section verbatim from the query, then append
 * one A-record answer pointing to 192.168.4.1.
 */

#define DNS_MAX_PACKET 512
#define DNS_HDR_LEN    12

/* DNS header flags: QR=1 (response), AA=1 (authoritative), RCODE=0 */
#define DNS_FLAGS_RESPONSE 0x8400

static int build_response(const uint8_t *query, int qlen,
                           uint8_t *resp, int resp_max)
{
    if (qlen < DNS_HDR_LEN) return -1;

    /* Copy the full query (header + question) as the response base. */
    if (qlen > resp_max - 16) return -1;   /* 16 bytes for the answer RR */
    memcpy(resp, query, qlen);

    /* Patch header flags: QR=1, AA=1, RCODE=0. */
    uint16_t flags = htons(DNS_FLAGS_RESPONSE);
    memcpy(resp + 2, &flags, 2);

    /* Set ANCOUNT = 1 (big-endian). */
    resp[6] = 0x00;
    resp[7] = 0x01;

    /* Append the answer RR. */
    uint8_t *p = resp + qlen;

    /* NAME: pointer to the question QNAME (offset 12 from start of message). */
    *p++ = 0xC0;
    *p++ = 0x0C;

    /* TYPE = A (0x0001) */
    *p++ = 0x00; *p++ = 0x01;

    /* CLASS = IN (0x0001) */
    *p++ = 0x00; *p++ = 0x01;

    /* TTL = 60 seconds */
    *p++ = 0x00; *p++ = 0x00; *p++ = 0x00; *p++ = 0x3C;

    /* RDLENGTH = 4 (IPv4 address) */
    *p++ = 0x00; *p++ = 0x04;

    /* RDATA: 192.168.4.1 */
    *p++ = 192; *p++ = 168; *p++ = 4; *p++ = 1;

    return (int)(p - resp);
}

static void dns_hijack_task(void *pvParam)
{
    (void)pvParam;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        ESP_LOGE(TAG, "DNS: socket() failed: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    /* Allow address reuse so a restart doesn't fail with EADDRINUSE. */
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in bind_addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(53),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    if (bind(sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        ESP_LOGE(TAG, "DNS: bind(53) failed: errno %d", errno);
        close(sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "DNS hijack task running on UDP port 53");

    static uint8_t rx[DNS_MAX_PACKET];
    static uint8_t tx[DNS_MAX_PACKET];

    for (;;) {
        struct sockaddr_in client;
        socklen_t clen = sizeof(client);
        int n = recvfrom(sock, rx, sizeof(rx), 0,
                         (struct sockaddr *)&client, &clen);
        if (n < DNS_HDR_LEN) continue;

        /* Only respond to queries (QR bit = 0). */
        uint16_t flags;
        memcpy(&flags, rx + 2, 2);
        if (ntohs(flags) & 0x8000) continue;   /* already a response */

        int rlen = build_response(rx, n, tx, sizeof(tx));
        if (rlen > 0) {
            sendto(sock, tx, rlen, 0,
                   (struct sockaddr *)&client, clen);
        }
    }
    /* Never reached. */
    close(sock);
    vTaskDelete(NULL);
}

esp_err_t dns_hijack_start(void)
{
    BaseType_t ret = xTaskCreate(dns_hijack_task, "dns_hijack",
                                 4096, NULL, 5, NULL);
    if (ret != pdPASS) {
        ESP_LOGE(TAG, "DNS: xTaskCreate failed");
        return ESP_FAIL;
    }
    return ESP_OK;
}
