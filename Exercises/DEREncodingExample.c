#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int encode_asn1_length(uint8_t *buffer, size_t length)
{
    if (buffer == NULL) {
        return -1;
    }

    if (length < 0x80) {
        buffer[0] = (uint8_t)length;
        return 1;
    }

    /* This implementation only needs short-form length for values < 128. */
    return -1;
}

static int encode_asn1_integer_from_bytes(const uint8_t *value_bytes,
                                          size_t value_len,
                                          uint8_t *output,
                                          size_t *output_len)
{
    size_t index;
    size_t content_len;
    size_t total_len;
    uint8_t first_byte;

    if (value_bytes == NULL || output == NULL || output_len == NULL) {
        return -1;
    }

    /* Strip leading zero bytes from the input content. */
    index = 0;
    while (index < value_len && value_bytes[index] == 0x00) {
        ++index;
    }

    if (index == value_len) {
        /* Value is zero. Encode as a single zero byte. */
        content_len = 1;
        first_byte = 0x00;
    } else {
        content_len = value_len - index;
        first_byte = value_bytes[index];
    }

    if (first_byte & 0x80U) {
        /* Prepend a zero byte to force unsigned INTEGER representation. */
        total_len = 1 + 1 + content_len + 1;
        if (output != NULL) {
            output[0] = 0x02; /* INTEGER tag */
            output[1] = (uint8_t)(content_len + 1);
            output[2] = 0x00;
            memcpy(output + 3, value_bytes + index, content_len);
        }
        *output_len = 3 + content_len;
    } else {
        total_len = 1 + 1 + content_len;
        if (output != NULL) {
            output[0] = 0x02; /* INTEGER tag */
            output[1] = (uint8_t)content_len;
            if (index == value_len) {
                output[2] = 0x00;
            } else {
                memcpy(output + 2, value_bytes + index, content_len);
            }
        }
        *output_len = 2 + content_len;
    }

    (void)total_len;
    return 0;
}

static int encode_asn1_integer_uint64(uint64_t value,
                                      uint8_t **output,
                                      size_t *output_len)
{
    uint8_t value_bytes[8];
    size_t value_len = sizeof(value_bytes);
    int result;
    size_t encoded_len;
    uint8_t *buffer;

    if (output == NULL || output_len == NULL) {
        return -1;
    }

    for (size_t index = 0; index < sizeof(value_bytes); ++index) {
        value_bytes[sizeof(value_bytes) - 1 - index] = (uint8_t)(value & 0xFFU);
        value >>= 8;
    }

    /* Determine the maximum needed length: tag + length + 8 data bytes + maybe leading 0x00. */
    buffer = (uint8_t *)malloc(1 + 1 + sizeof(value_bytes) + 1);
    if (buffer == NULL) {
        return -1;
    }

    result = encode_asn1_integer_from_bytes(value_bytes, value_len, buffer, &encoded_len);
    if (result != 0) {
        free(buffer);
        return -1;
    }

    *output = buffer;
    *output_len = encoded_len;
    return 0;
}

static int encode_asn1_sequence(const uint8_t *content,
                                size_t content_len,
                                uint8_t **output,
                                size_t *output_len)
{
    uint8_t header[2];
    int header_len;
    uint8_t *buffer;

    if (output == NULL || output_len == NULL || content == NULL) {
        return -1;
    }

    header[0] = 0x30; /* SEQUENCE tag */
    header_len = encode_asn1_length(header + 1, content_len);
    if (header_len != 1) {
        return -1;
    }

    buffer = (uint8_t *)malloc(1 + header_len + content_len);
    if (buffer == NULL) {
        return -1;
    }

    buffer[0] = header[0];
    buffer[1] = header[1];
    memcpy(buffer + 2, content, content_len);

    *output = buffer;
    *output_len = 2 + content_len;
    return 0;
}

static int encode_rsa_public_key(uint64_t n,
                                 uint64_t e,
                                 uint8_t **output,
                                 size_t *output_len)
{
    uint8_t *n_enc = NULL;
    uint8_t *e_enc = NULL;
    uint8_t *content = NULL;
    size_t n_len;
    size_t e_len;
    size_t content_len;
    uint8_t *buffer;
    int result;

    if (output == NULL || output_len == NULL) {
        return -1;
    }

    result = encode_asn1_integer_uint64(n, &n_enc, &n_len);
    if (result != 0) {
        return -1;
    }

    result = encode_asn1_integer_uint64(e, &e_enc, &e_len);
    if (result != 0) {
        free(n_enc);
        return -1;
    }

    content_len = n_len + e_len;
    content = (uint8_t *)malloc(content_len);
    if (content == NULL) {
        free(n_enc);
        free(e_enc);
        return -1;
    }

    memcpy(content, n_enc, n_len);
    memcpy(content + n_len, e_enc, e_len);

    free(n_enc);
    free(e_enc);

    result = encode_asn1_sequence(content, content_len, &buffer, output_len);
    free(content);
    if (result != 0) {
        return -1;
    }

    *output = buffer;
    return 0;
}

static int encode_rsa_private_key(uint64_t n,
                                  uint64_t e,
                                  uint64_t d,
                                  uint8_t **output,
                                  size_t *output_len)
{
    uint8_t *version = NULL;
    uint8_t *n_enc = NULL;
    uint8_t *e_enc = NULL;
    uint8_t *d_enc = NULL;
    uint8_t *zero_int = NULL;
    size_t version_len;
    size_t n_len;
    size_t e_len;
    size_t d_len;
    size_t zero_len;
    size_t content_len;
    uint8_t *content = NULL;
    uint8_t *buffer;
    int result;

    if (output == NULL || output_len == NULL) {
        return -1;
    }

    result = encode_asn1_integer_uint64(0, &version, &version_len);
    if (result != 0) {
        return -1;
    }

    result = encode_asn1_integer_uint64(n, &n_enc, &n_len);
    if (result != 0) {
        free(version);
        return -1;
    }

    result = encode_asn1_integer_uint64(e, &e_enc, &e_len);
    if (result != 0) {
        free(version);
        free(n_enc);
        return -1;
    }

    result = encode_asn1_integer_uint64(d, &d_enc, &d_len);
    if (result != 0) {
        free(version);
        free(n_enc);
        free(e_enc);
        return -1;
    }

    result = encode_asn1_integer_uint64(0, &zero_int, &zero_len);
    if (result != 0) {
        free(version);
        free(n_enc);
        free(e_enc);
        free(d_enc);
        return -1;
    }

    content_len = version_len + n_len + e_len + d_len + zero_len * 5;
    content = (uint8_t *)malloc(content_len);
    if (content == NULL) {
        free(version);
        free(n_enc);
        free(e_enc);
        free(d_enc);
        free(zero_int);
        return -1;
    }

    size_t offset = 0;
    memcpy(content + offset, version, version_len);
    offset += version_len;
    memcpy(content + offset, n_enc, n_len);
    offset += n_len;
    memcpy(content + offset, e_enc, e_len);
    offset += e_len;
    memcpy(content + offset, d_enc, d_len);
    offset += d_len;

    for (int i = 0; i < 5; ++i) {
        memcpy(content + offset, zero_int, zero_len);
        offset += zero_len;
    }

    free(version);
    free(n_enc);
    free(e_enc);
    free(d_enc);
    free(zero_int);

    result = encode_asn1_sequence(content, content_len, &buffer, output_len);
    free(content);
    if (result != 0) {
        return -1;
    }

    *output = buffer;
    return 0;
}

int rsa_der_encode(uint64_t n,
                   uint64_t e,
                   uint64_t d,
                   uint8_t **public_key_der,
                   size_t *public_key_len,
                   uint8_t **private_key_der,
                   size_t *private_key_len)
{
    if (public_key_der == NULL || public_key_len == NULL ||
        private_key_der == NULL || private_key_len == NULL) {
        return -1;
    }

    *public_key_der = NULL;
    *private_key_der = NULL;
    *public_key_len = 0;
    *private_key_len = 0;

    if (encode_rsa_public_key(n, e, public_key_der, public_key_len) != 0) {
        return -1;
    }

    if (encode_rsa_private_key(n, e, d, private_key_der, private_key_len) != 0) {
        free(*public_key_der);
        *public_key_der = NULL;
        *public_key_len = 0;
        return -1;
    }

    return 0;
}

static void print_hex(const uint8_t *data, size_t len)
{
    if (data == NULL) {
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        printf("%02X", data[i]);
        if ((i + 1) % 16 == 0 && i + 1 < len) {
            printf("\n");
        } else if (i + 1 < len) {
            printf(" ");
        }
    }
    printf("\n");
}

int main(void)
{
    uint64_t n = 18446743979220271189ULL; /* Example modulus in hex */
    uint64_t e = 65537ULL; /* Common public exponent (65537) */
    uint64_t d = 9331878932546167513ULL; /* Example private exponent in hex */
    uint8_t *public_key = NULL;
    uint8_t *private_key = NULL;
    size_t public_key_len = 0;
    size_t private_key_len = 0;

    if (rsa_der_encode(n, e, d,
                       &public_key, &public_key_len,
                       &private_key, &private_key_len) != 0) {
        fprintf(stderr, "RSA DER encoding failed.\n");
        return EXIT_FAILURE;
    }

    printf("Public Key DER (%zu bytes):\n", public_key_len);
    print_hex(public_key, public_key_len);
    printf("\nPrivate Key DER (%zu bytes):\n", private_key_len);
    print_hex(private_key, private_key_len);

    free(public_key);
    free(private_key);
    return EXIT_SUCCESS;
}
