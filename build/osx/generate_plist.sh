#!/bin/bash

set -e

# --- CONFIG ---
VERSION_H="${SRCROOT}/../../source/plugins/${TARGET_NAME}/version.h"
PLIST="$TARGET_BUILD_DIR/$INFOPLIST_PATH"
# --------------

echo "🔍 Reading version from: $VERSION_H"
echo "📄 Info.plist path: $PLIST"

# Extract VERSION_STRING
if [[ ! -f "$VERSION_H" ]]; then
    echo "❌ ERROR: version.h not found at $VERSION_H"
    exit 1
fi

VERSION=$(grep '#define VERSION_STRING' "$VERSION_H" | cut -d '"' -f2)

if [[ -z "$VERSION" ]]; then
    echo "❌ ERROR: VERSION_STRING not found in $VERSION_H"
    exit 1
fi

echo "✅ Found version: $VERSION"

# Create Info.plist if missing
if [[ ! -f "$PLIST" ]]; then
    echo "🆕 Creating Info.plist at $PLIST"
    mkdir -p "$(dirname "$PLIST")"
    cat > "$PLIST" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" \
    "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleIdentifier</key>
    <string>com.hybridconstructs.${TARGET_NAME}</string>
</dict>
</plist>
EOF
fi

# Set version fields
/usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString $VERSION" "$PLIST" 2>/dev/null || \
    /usr/libexec/PlistBuddy -c "Add :CFBundleShortVersionString string $VERSION" "$PLIST"

/usr/libexec/PlistBuddy -c "Set :CFBundleVersion $VERSION" "$PLIST" 2>/dev/null || \
    /usr/libexec/PlistBuddy -c "Add :CFBundleVersion string $VERSION" "$PLIST"

echo "✅ Info.plist updated successfully"
