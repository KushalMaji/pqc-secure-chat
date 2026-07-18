# Post-Quantum Secure Chat using ML-KEM-768

A secure client-server chat application that uses **Post-Quantum Cryptography (PQC)** for key exchange and **AES-256** for encrypted communication.

This project demonstrates how to establish a secure communication channel using **ML-KEM-768 (Kyber)** from the Open Quantum Safe (liboqs) library and then encrypt chat messages using **AES-256**.

---

## Features

- Post-Quantum Key Exchange using ML-KEM-768
- AES-256 encrypted communication
- SHA-256 based session key derivation
- TCP client-server architecture
- Interactive encrypted chat
- Error handling and memory management
- Built using C, OpenSSL, and liboqs

---

## Technologies Used

- C Programming
- liboqs (Open Quantum Safe)
- OpenSSL
- TCP Sockets
- Linux (Ubuntu/SeedLabs)

---

## Project Structure

```
secure_chat/
│── final_server.c
│── final_client.c
│── crypto.c
│── crypto.h
│── Makefile
│── README.md
```

---

## Cryptographic Workflow

1. Client generates an ML-KEM-768 key pair.
2. Client sends its public key to the server.
3. Server encapsulates a shared secret using the public key.
4. Server sends the ciphertext to the client.
5. Client decapsulates the ciphertext to recover the shared secret.
6. Both client and server derive an AES-256 session key using SHA-256.
7. All chat messages are encrypted using AES-256 before transmission.

---

## Build

Compile the project using:

```bash
make
```

or

```bash
gcc final_server.c crypto.c -loqs -lssl -lcrypto -o server
gcc final_client.c crypto.c -loqs -lssl -lcrypto -o client
```

---

## Running

Start the server:

```bash
./server
```

In another terminal, start the client:

```bash
./client
```

Begin exchanging encrypted messages.

---

## Security Components

| Component | Algorithm |
|----------|-----------|
| Key Exchange | ML-KEM-768 |
| Key Derivation | SHA-256 |
| Symmetric Encryption | AES-256 |
| Transport | TCP |

---

## Future Improvements

- ML-DSA (Dilithium) for authentication
- Protection against Man-in-the-Middle attacks
- Random IV generation
- AES-GCM authenticated encryption
- Multi-client support

---

## License

This project is intended for educational and research purposes.
