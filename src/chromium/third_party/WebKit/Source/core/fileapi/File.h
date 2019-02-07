// -------------------------------------------------
// BlinKit - blink Library
// -------------------------------------------------
//   File Name: File.h
// Description: File Class
//      Author: Ziming Li
//     Created: 2019-02-07
// -------------------------------------------------
// Copyright (C) 2019 MingYang Software Technology.
// -------------------------------------------------

/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef File_h
#define File_h

#include "core/CoreExport.h"
#include "core/fileapi/Blob.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;
class ExecutionContext;
class FilePropertyBag;
class FileMetadata;
class KURL;

class CORE_EXPORT File final : public Blob {
    DEFINE_WRAPPERTYPEINFO();
public:
    // AllContentTypes should only be used when the full path/name are trusted; otherwise, it could
    // allow arbitrary pages to determine what applications an user has installed.
    enum ContentTypeLookupPolicy {
        WellKnownContentTypes,
        AllContentTypes,
    };

    // The user should not be able to browse to some files, such as the ones
    // generated by the Filesystem API.
    enum UserVisibility { IsUserVisible, IsNotUserVisible };

    static File* create(const String& path, ContentTypeLookupPolicy policy = WellKnownContentTypes)
    {
        return new File(path, policy, File::IsUserVisible);
    }

    static File* create(const String& name, double modificationTime, PassRefPtr<BlobDataHandle> blobDataHandle)
    {
        return new File(name, modificationTime, blobDataHandle);
    }

    static File* create(const unsigned char* data, size_t bytes, const String& mimeType);

    // For deserialization.
    static File* createFromSerialization(const String& path, const String& name, const String& relativePath, UserVisibility userVisibility, bool hasSnaphotData, uint64_t size, double lastModified, PassRefPtr<BlobDataHandle> blobDataHandle)
    {
        return new File(path, name, relativePath, userVisibility, hasSnaphotData, size, lastModified, blobDataHandle);
    }
    static File* createFromIndexedSerialization(const String& path, const String& name, uint64_t size, double lastModified, PassRefPtr<BlobDataHandle> blobDataHandle)
    {
        return new File(path, name, String(), IsNotUserVisible, true, size, lastModified, blobDataHandle);
    }

    static File* createWithRelativePath(const String& path, const String& relativePath);

    // If filesystem files live in the remote filesystem, the port might pass the valid metadata (whose length field is non-negative) and cache in the File object.
    //
    // Otherwise calling size(), lastModifiedTime() and slice() will synchronously query the file metadata.
    static File* createForFileSystemFile(const String& name, const FileMetadata& metadata, UserVisibility userVisibility)
    {
        return new File(name, metadata, userVisibility);
    }

    static File* createForFileSystemFile(const KURL& url, const FileMetadata& metadata, UserVisibility userVisibility)
    {
        return new File(url, metadata, userVisibility);
    }

    KURL fileSystemURL() const { ASSERT(hasValidFileSystemURL()); return m_fileSystemURL; }

    // Create a file with a name exposed to the author (via File.name and associated DOM properties) that differs from the one provided in the path.
    static File* createForUserProvidedFile(const String& path, const String& displayName)
    {
        if (displayName.isEmpty())
            return new File(path, File::AllContentTypes, File::IsUserVisible);
        return new File(path, displayName, File::AllContentTypes, File::IsUserVisible);
    }

    static File* createForFileSystemFile(const String& path, const String& name, ContentTypeLookupPolicy policy = WellKnownContentTypes)
    {
        if (name.isEmpty())
            return new File(path, policy, File::IsNotUserVisible);
        return new File(path, name, policy, File::IsNotUserVisible);
    }

    File* clone(const String& name = String()) const;

    unsigned long long size() const override;
    Blob* slice(long long start, long long end, const String& contentType, ExceptionState&) const override;
    void close(ExecutionContext*, ExceptionState&) override;

    bool isFile() const override { return true; }
    bool hasBackingFile() const override { return m_hasBackingFile; }

    void appendTo(BlobData&) const override;

    const String& path() const { ASSERT(hasValidFilePath()); return m_path; }
    const String& name() const { return m_name; }

    // Getter for the lastModified IDL attribute,
    // http://dev.w3.org/2006/webapi/FileAPI/#file-attrs
    long long lastModified() const;

    // Getter for the lastModifiedDate IDL attribute,
    // http://www.w3.org/TR/FileAPI/#dfn-lastModifiedDate
    double lastModifiedDate() const;

    UserVisibility userVisibility() const { return m_userVisibility; }

    // Returns the relative path of this file in the context of a directory selection.
    const String& webkitRelativePath() const { return m_relativePath; }

    // Note that this involves synchronous file operation. Think twice before calling this function.
    void captureSnapshot(long long& snapshotSize, double& snapshotModificationTimeMS) const;

    // Returns true if this has a valid snapshot metadata (i.e. m_snapshotSize >= 0).
    bool hasValidSnapshotMetadata() const { return m_snapshotSize >= 0; }

    // Returns true if the sources (file path, file system URL, or blob handler) of the file objects are same or not.
    bool hasSameSource(const File& other) const;

private:
    File(const String& path, ContentTypeLookupPolicy, UserVisibility);
    File(const String& path, const String& name, ContentTypeLookupPolicy, UserVisibility);
    File(const String& path, const String& name, const String& relativePath, UserVisibility, bool hasSnaphotData, uint64_t size, double lastModified, PassRefPtr<BlobDataHandle>);
    File(const String& name, double modificationTime, PassRefPtr<BlobDataHandle>);
    File(const String& name, const FileMetadata&, UserVisibility);
    File(const KURL& fileSystemURL, const FileMetadata&, UserVisibility);
    File(const File&);

    void invalidateSnapshotMetadata() { m_snapshotSize = -1; }

    // Returns File's last modified time (in MS since Epoch.)
    // If the modification time isn't known, the current time is returned.
    double lastModifiedMS() const;

#if ENABLE(ASSERT)
    // Instances backed by a file must have an empty file system URL.
    bool hasValidFileSystemURL() const { return !hasBackingFile() || m_fileSystemURL.isEmpty(); }
    // Instances not backed by a file must have an empty path set.
    bool hasValidFilePath() const { return hasBackingFile() || m_path.isEmpty(); }
#endif

    bool m_hasBackingFile;
    UserVisibility m_userVisibility;
    String m_path;
    String m_name;

    KURL m_fileSystemURL;

    // If m_snapshotSize is negative (initialized to -1 by default), the snapshot metadata is invalid and we retrieve the latest metadata synchronously in size(), lastModifiedTime() and slice().
    // Otherwise, the snapshot metadata are used directly in those methods.
    long long m_snapshotSize;
    const double m_snapshotModificationTimeMS;

    String m_relativePath;
};

DEFINE_TYPE_CASTS(File, Blob, blob, blob->isFile(), blob.isFile());

} // namespace blink

#endif // File_h
