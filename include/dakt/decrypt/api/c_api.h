#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DaktDecryptContext *DaktDecryptContextHandle;
typedef struct DaktDecryptReport *DaktDecryptReportHandle;

enum DaktDecryptResult {
  DAKT_DECRYPT_OK = 0,
  DAKT_DECRYPT_ERR_IO,
  DAKT_DECRYPT_ERR_UNRECOGNIZED,
  DAKT_DECRYPT_ERR_DECRYPT,
  DAKT_DECRYPT_ERR_DECOMPRESS
};

#ifdef __cplusplus
}
#endif
