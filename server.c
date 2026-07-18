#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <oqs/oqs.h>

#include <openssl/sha.h>

#include "crypto.h"

#define PORT 8080

int main()
{
    int server_fd;
    int client_fd;

    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    printf("=========================================\n");
    printf(" POST-QUANTUM SECURE CHAT SERVER\n");
    printf("=========================================\n\n");

    /* ---------- ML-KEM ---------- */

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_768);

    if (kem == NULL)
    {
        printf("ML-KEM-768 not available!\n");
        return 1;
    }

    uint8_t *public_key =
        malloc(kem->length_public_key);

    uint8_t *ciphertext =
        malloc(kem->length_ciphertext);

    uint8_t *shared_secret =
        malloc(kem->length_shared_secret);

if (public_key == NULL ||
    ciphertext == NULL ||
    shared_secret == NULL)
{
    printf("Memory allocation failed.\n");
    OQS_KEM_free(kem);
    return 1;
}

    unsigned char aes_key[32];

    /* ---------- Socket ---------- */

server_fd = socket(AF_INET,
                   SOCK_STREAM,
                   0);

if (server_fd < 0)
{
    printf("Socket creation failed.\n");
    return 1;
}

int opt = 1;

setsockopt(server_fd,
           SOL_SOCKET,
           SO_REUSEADDR,
           &opt,
           sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

if (bind(server_fd,
         (struct sockaddr *)&address,
         sizeof(address)) < 0)
{
    printf("Bind failed.\n");
    return 1;
}

if (listen(server_fd, 1) < 0)
{
    printf("Listen failed.\n");
    return 1;
}

    printf("Waiting for client...\n\n");

    client_fd = accept(server_fd,
                       (struct sockaddr *)&address,
                       &addrlen);

if (client_fd < 0)
{
    printf("Accept failed.\n");
    return 1;
}

    printf("Client Connected.\n\n");

    /* ---------- Receive Public Key ---------- */

 if (recv(client_fd,
         public_key,
         kem->length_public_key,
         MSG_WAITALL) <= 0)
{
    printf("Failed to receive public key.\n");
    return 1;
}

    printf("Public Key Received.\n");

    /* ---------- Encapsulation ---------- */

    if (OQS_KEM_encaps(
        kem,
        ciphertext,
        shared_secret,
        public_key) != OQS_SUCCESS)
{
    printf("ML-KEM encapsulation failed.\n");
    return 1;
}

 if (send(client_fd,
         ciphertext,
         kem->length_ciphertext,
         0) <= 0)
{
    printf("Failed to send ciphertext.\n");
    return 1;
}

    printf("Ciphertext Sent.\n");

    /* ---------- SHA256 ---------- */

    if (SHA256(shared_secret,
           kem->length_shared_secret,
           aes_key) == NULL)
{
    printf("SHA-256 key derivation failed.\n");
    return 1;
}

    printf("\nAES-256 Session Key\n");
    printf("--------------------------------------\n");

    for(int i=0;i<32;i++)
    {
        printf("%02X",aes_key[i]);

        if((i+1)%16==0)
            printf("\n");
    }

    printf("\nSecure Channel Established.\n");
    printf("\n=========================================\n");
    printf(" Secure Chat Started\n");
    printf(" Type your reply after receiving a message.\n");
    printf(" Type 'exit' to close the chat.\n");
    printf("=========================================\n");

    while (1)
    {
        unsigned char ciphertext_msg[2048];
        unsigned char plaintext[2048];
        unsigned char iv[16] = {0};

        int cipher_len;

        /* Receive encrypted message length */
if (recv(client_fd,
         &cipher_len,
         sizeof(int),
         MSG_WAITALL) <= 0)
{
    printf("Client disconnected.\n");
    break;
}

if (cipher_len <= 0 || cipher_len > sizeof(ciphertext_msg))
{
    printf("Invalid ciphertext length received.\n");
    break;
}

        /* Receive encrypted message */
        if (recv(client_fd,
                 ciphertext_msg,
                 cipher_len,
                 MSG_WAITALL) <= 0)
        {
            printf("\nClient disconnected.\n");
            break;
        }

        printf("\n=========================================\n");
        printf("Encrypted Message (Hex)\n");
        printf("=========================================\n");

        for (int i = 0; i < cipher_len; i++)
        {
            printf("%02X ", ciphertext_msg[i]);

            if ((i + 1) % 16 == 0)
                printf("\n");
        }

        printf("\n");

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

        printf("\n-----------------------------------------\n");
        printf("Decrypted Message:\n");
        printf("%s\n", plaintext);
        printf("-----------------------------------------\n");

        if (strcmp((char *)plaintext, "exit") == 0)
        {
            printf("\nClient ended the chat.\n");
            break;
        }

        char reply[1024];

printf("\nYou: ");
if (fgets(reply, sizeof(reply), stdin) == NULL)
{
    break;
}

reply[strcspn(reply, "\n")] = 0;

if (strlen(reply) == 0)
{
    continue;
}

cipher_len = encrypt(
    (unsigned char *)reply,
    strlen(reply),
    aes_key,
    iv,
    ciphertext_msg
);

if (cipher_len < 0)
{
    printf("Encryption failed.\n");
    continue;
}

printf("\n=========================================\n");
printf("Encrypted Reply (Hex)\n");
printf("=========================================\n");

for (int i = 0; i < cipher_len; i++)
{
    printf("%02X ", ciphertext_msg[i]);

    if ((i + 1) % 16 == 0)
        printf("\n");
}

printf("\n");

if (send(client_fd,
         &cipher_len,
         sizeof(int),
         0) <= 0)
{
    printf("Send failed.\n");
    break;
}

if (send(client_fd,
         ciphertext_msg,
         cipher_len,
         0) <= 0)
{
    printf("Send failed.\n");
    break;
}

        if (strcmp(reply, "exit") == 0)
        {
            printf("\nClosing chat...\n");
            break;
        }
    }

    free(public_key);
    free(ciphertext);
    free(shared_secret);

    OQS_KEM_free(kem);

    close(client_fd);
    close(server_fd);

    printf("\nServer Closed Successfully.\n");

    return 0;
}
