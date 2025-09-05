#!/bin/bash
source ./version.sh
curl --user tesses50:$GITEA_AUTH -X DELETE \
     https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/tessesframework/$DEB_VERSION/amd64
curl --user tesses50:$GITEA_AUTH -X DELETE \
     https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/tessesframework/$DEB_VERSION/arm64
curl --user tesses50:$GITEA_AUTH -X DELETE \
     https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/tessesframework/$DEB_VERSION/riscv64
curl --user tesses50:$GITEA_AUTH -X DELETE \
     https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/tessesframework/$DEB_VERSION/armhf
curl --user tesses50:$GITEA_AUTH \
    --upload-file build/jammy/tessesframework_$DEB_VERSION\_amd64.deb \
    https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/upload
curl --user tesses50:$GITEA_AUTH \
    --upload-file build/jammy/tessesframework_$DEB_VERSION\_arm64.deb \
    https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/upload
curl --user tesses50:$GITEA_AUTH \
    --upload-file build/jammy/tessesframework_$DEB_VERSION\_riscv64.deb \
    https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/upload
curl --user tesses50:$GITEA_AUTH \
    --upload-file build/jammy/tessesframework_$DEB_VERSION\_armhf.deb \
    https://git.tesseslanguage.com/api/packages/tesses50/debian/pool/jammy/main/upload