#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <oqs/oqs.h>

#define PORT 8080

int main() {

    int sock;
    struct sockaddr_in server_addr;

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);

    if (kem == NULL) {
        printf("ML-KEM not available!\n");
        return 1;
    }

    uint8_t *public_key = malloc(kem->length_public_key);
    uint8_t *secret_key = malloc(kem->length_secret_key);
    uint8_t *ciphertext = malloc(kem->length_ciphertext);
    uint8_t *shared_secret = malloc(kem->length_shared_secret);

    OQS_KEM_keypair(
        kem,
        public_key,
        secret_key
    );

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    inet_pton(AF_INET,
              "127.0.0.1",
              &server_addr.sin_addr);

    connect(sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr));

    printf("Connected to server.\n");

    send(sock,
         public_key,
         kem->length_public_key,
         0);

    recv(sock,
         ciphertext,
         kem->length_ciphertext,
         0);

    OQS_KEM_decaps(
        kem,
        shared_secret,
        ciphertext,
        secret_key
    );

    printf("\nRecovered Shared Secret:\n");

    for (size_t i = 0; i < 32; i++) {
        printf("%02X", shared_secret[i]);
    }

    printf("\n");

    free(public_key);
    free(secret_key);
    free(ciphertext);
    free(shared_secret);

    OQS_KEM_free(kem);

    close(sock);

    return 0;
}
