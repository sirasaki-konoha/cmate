#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
typedef BOOLEAN (APIENTRY *RtlGenRandomFunc)(PVOID, ULONG);

char *generate_guid_v4() {
    uint8_t uuid[16];
    HMODULE advapi = GetModuleHandleA("advapi32.dll");
    if (!advapi) return NULL;

    RtlGenRandomFunc RtlGenRandom = (RtlGenRandomFunc)GetProcAddress(advapi, "SystemFunction036");
    if (!RtlGenRandom || !RtlGenRandom(uuid, 16)) return NULL;

    // version と variant をセット
    uuid[6] = (uuid[6] & 0x0F) | 0x40;
    uuid[8] = (uuid[8] & 0x3F) | 0x80;

    char *guid = malloc(37);
    if (!guid) return NULL;

    snprintf(guid, 37,
             "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
             uuid[0], uuid[1], uuid[2], uuid[3],
             uuid[4], uuid[5],
             uuid[6], uuid[7],
             uuid[8], uuid[9],
             uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);

    return guid;
}

#else 

char *generate_guid_v4() {
    static const char *format = "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x";
    unsigned char bytes[16];
    FILE *f = fopen("/dev/urandom", "rb");
    if (!f) return NULL;
    fread(bytes, 1, 16, f);
    fclose(f);

    // Set version (4) and variant (RFC 4122)
    bytes[6] = (bytes[6] & 0x0F) | 0x40;
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    char *guid = malloc(37); // 36 + null terminator
    if (!guid) return NULL;
    snprintf(guid, 37, format,
             bytes[0], bytes[1], bytes[2], bytes[3],
             bytes[4], bytes[5],
             bytes[6], bytes[7],
             bytes[8], bytes[9],
             bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);

    return guid;
}

#endif /* _WIN32 */


