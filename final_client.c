#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <oqs/oqs.h>
#include <string.h>
#include "crypto.h"
#include <openssl/sha.h>

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
    unsigned char aes_key[32];

    if (public_key == NULL ||
        secret_key == NULL ||
        ciphertext == NULL ||
        shared_secret == NULL)
    {
        printf("Memory allocation failed.\n");
        OQS_KEM_free(kem);
        return 1;
    }

    if (OQS_KEM_keypair(kem,
                    public_key,
                    secret_key) != OQS_SUCCESS)
    {
        printf("ML-KEM keypair generation failed.\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        printf("Socket creation failed.\n");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET,
              "127.0.0.1",
              &server_addr.sin_addr) <= 0)
    {
        printf("Invalid server address.\n");
        return 1;
    }

    if (connect(sock,
            (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0)
    {
        printf("Connection failed.\n");
        return 1;
    }

    printf("=================================\n");
    printf("Post-Quantum Secure Client\n");
    printf("=================================\n");

    printf("Connected to server.\n");

    if (send(sock,
         public_key,
         kem->length_public_key,
         0) <= 0)
    {
        printf("Failed to send public key.\n");
        return 1;
    }

    printf("Public key sent.\n");

    if (recv(sock,
         ciphertext,
         kem->length_ciphertext,
         MSG_WAITALL) <= 0)
    {
        printf("Failed to receive ciphertext.\n");
        return 1;
    }

    printf("Ciphertext received.\n");

    if (OQS_KEM_decaps(
        kem,
        shared_secret,
        ciphertext,
        secret_key) != OQS_SUCCESS)
    {
        printf("ML-KEM decapsulation failed.\n");
        return 1;
    }

    if (SHA256(shared_secret,
               kem->length_shared_secret,
               aes_key) == NULL)
    {
        printf("SHA-256 key derivation failed.\n");
        return 1;
    }

    printf("\nRecovered Shared Secret (first 16 bytes):\n");

    for (int i = 0; i < 16; i++)
        printf("%02X", shared_secret[i]);

    printf("\n");

    printf("\nSecure channel established.\n");

char message[1024];

while (1)
{
    printf("\nYou: ");
    if (fgets(message,
          sizeof(message),
          stdin) == NULL)
    {
        break;
    }

    message[strcspn(message, "\n")] = 0;

    if(strcmp(message, "exit") == 0)
        break;

    unsigned char ciphertext_msg[2048];
    unsigned char plaintext[2048];
    unsigned char iv[16] = {0};

    int cipher_len = encrypt(
        (unsigned char*)message,
        strlen(message),
        aes_key,
        iv,
        ciphertext_msg
    );

    if (cipher_len < 0)
    {
        printf("Encryption failed.\n");
        continue;
    }

    if (send(sock,
         &cipher_len,
         sizeof(int),
         0) <= 0)
    {
        printf("Send failed.\n");
        break;
    }

    if (send(sock,
             ciphertext_msg,
             cipher_len,
             0) <= 0)
    {
        printf("Send failed.\n");
        break;
    }

    if (recv(sock,
             &cipher_len,
             sizeof(int),
             MSG_WAITALL) <= 0)
    {
        printf("Server disconnected.\n");
        break;
    }

    if (cipher_len <= 0 || cipher_len > sizeof(ciphertext_msg))
    {
        printf("Invalid ciphertext length.\n");
        break;
    }

    if (recv(sock,
         ciphertext_msg,
         cipher_len,
         MSG_WAITALL) <= 0)
    {
        printf("Server disconnected.\n");
        break;
    }

    int plain_len = decrypt(
    ciphertext_msg,
    cipher_len,
    aes_key,
    iv,
    plaintext
    );

    if (plain_len < 0)
    {
        printf("Decryption failed.\n");
        continue;
    }

    plaintext[plain_len] = '\0';

    printf("Server: %s\n", plaintext);
}

    free(public_key);
    free(secret_key);
    free(ciphertext);
    free(shared_secret);

    OQS_KEM_free(kem);

    close(sock);

    return 0;
}
