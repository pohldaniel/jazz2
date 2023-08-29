#pragma once

#include "Stream.h"
#include "../Common.h"
#include "../Containers/String.h"
#include "../Containers/StringView.h"

#include <memory>
#include <optional>

#if !defined(DEATH_TARGET_WINDOWS)
#	include <climits> // for `PATH_MAX`
#	if defined(DEATH_TARGET_APPLE) || defined(DEATH_TARGET_SWITCH)
#		include <dirent.h>
#	elif defined(DEATH_TARGET_ANDROID)
using DIR = struct DIR;
using AAssetDir = struct AAssetDir;
#	elif defined(__FreeBSD__)
struct _dirdesc;
using DIR = struct _dirdesc;
#	else
struct __dirstream;
using DIR = struct __dirstream;
#	endif
#endif

namespace Death::IO
{
	/** @brief File system related methods */
	class FileSystem
	{
	public:
#if defined(DEATH_TARGET_WINDOWS)
		static constexpr std::uint32_t MaxPathLength = /*MAX_PATH*/260;
		static constexpr char PathSeparator[] = "\\";
#else
		static constexpr std::uint32_t MaxPathLength = PATH_MAX;
		static constexpr char PathSeparator[] = "/";
#endif

		/** @brief The available permissions to check or set */
		enum class Permission
		{
			None = 0,

			Read = 0x01,
			Write = 0x02,
			Execute = 0x04
		};

		DEFINE_PRIVATE_ENUM_OPERATORS(Permission);

		struct FileDate
		{
			std::int32_t Year;
			std::int32_t Month;
			std::int32_t Day;
			std::int32_t Hour;
			std::int32_t Minute;
			std::int32_t Second;

			std::uint64_t Ticks;
		};

		enum class EnumerationOptions
		{
			None = 0,

			/** @brief Skip regular files */
			SkipFiles = 0x01,
			/** @brief Skip directories */
			SkipDirectories = 0x02,
			/** @brief Skip everything that is not a file or directory */
			SkipSpecial = 0x04
		};

		DEFINE_PRIVATE_ENUM_OPERATORS(EnumerationOptions);

		/** @brief The class that handles directory traversal */
		class Directory
		{
		public:
			Directory(const Containers::StringView& path, EnumerationOptions options = EnumerationOptions::None);
			~Directory();

			/** @brief Opens a directory for traversal */
			bool Open(const Containers::StringView& path, EnumerationOptions options = EnumerationOptions::None);
			/** @brief Closes an opened directory */
			void Close();
			/** @brief Returns the name of the next file inside the directory or `nullptr` */
			const char* GetNext();

		private:
			EnumerationOptions _options;

			char _path[MaxPathLength];
			char* _fileNamePart = nullptr;
#if defined(DEATH_TARGET_WINDOWS)
			bool _firstFile = true;
			void* _hFindFile = NULL;
#else
#	if defined(DEATH_TARGET_ANDROID)
			AAssetDir* _assetDir = nullptr;
#	endif
			DIR* _dirStream = nullptr;
#endif
		};

#if defined(DEATH_TARGET_WINDOWS) || defined(DEATH_TARGET_SWITCH)
		// Windows is already case in-sensitive
		DEATH_ALWAYS_INLINE static const Containers::StringView& FindPathCaseInsensitive(const Containers::StringView& path) {
			return path;
		}
#else
		static Containers::String FindPathCaseInsensitive(const Containers::StringView& path);
#endif

		/** @brief Combines together two path components */
		static Containers::String CombinePath(const Containers::StringView& first, const Containers::StringView& second);
		static Containers::String CombinePath(const Containers::ArrayView<const Containers::StringView> paths);
		static Containers::String CombinePath(const std::initializer_list<Containers::StringView> paths);

		/** @brief Returns the path up to, but not including, the final separator */
		static Containers::StringView GetDirectoryName(const Containers::StringView& path);
		/** @brief Returns the path component after the final separator */
		static Containers::StringView GetFileName(const Containers::StringView& path);
		/** @brief Returns the path component after the final separator without extension */
		static Containers::StringView GetFileNameWithoutExtension(const Containers::StringView& path);
		/** @brief Returns the extension as lower-case string without dot or empty string if it is not found */
		static Containers::String GetExtension(const Containers::StringView& path);
		/** @brief Converts path using forward slashes to native separators */
#if defined(DEATH_TARGET_WINDOWS)
		static Containers::String ToNativeSeparators(Containers::String path);
#else
		DEATH_ALWAYS_INLINE static const Containers::StringView& ToNativeSeparators(const Containers::StringView& path) {
			return path;
		}
#endif

		/** @brief Returns an absolute path from a relative one */
		static Containers::String GetAbsolutePath(const Containers::StringView& path);

		/** @brief Returns the path of executable */
		static Containers::String GetExecutablePath();
		/** @brief Returns the path of current working directory */
		static Containers::String GetWorkingDirectory();
		/** @brief Sets the current working directory, the starting point for interpreting relative paths */
		static bool SetWorkingDirectory(const Containers::StringView& path);
		/** @brief Returns the path of the user home directory */
		static Containers::String GetHomeDirectory();
#if defined(DEATH_TARGET_ANDROID)
		/** @brief Returns the path of the Android external storage directory */
		static Containers::String GetExternalStorage();
#elif defined(DEATH_TARGET_UNIX)
		static Containers::String GetLocalStorage();
#endif

		/** @brief Returns true if the specified path is a directory */
		static bool DirectoryExists(const Containers::StringView& path);
		/** @brief Returns true if the specified path is a file */
		static bool FileExists(const Containers::StringView& path);

		/** @brief Returns true if the file or directory exists */
		static bool Exists(const Containers::StringView& path);
		/** @brief Returns true if the file or directory is readable */
		static bool IsReadable(const Containers::StringView& path);
		/** @brief Returns true if the file or directory is writeable */
		static bool IsWritable(const Containers::StringView& path);
		/** @brief Returns true if the file or directory is executable */
		static bool IsExecutable(const Containers::StringView& path);

		/** @brief Returns true if the path is a file and is readable */
		static bool IsReadableFile(const Containers::StringView& path);
		/** @brief Returns true if the path is a file and is writeable */
		static bool IsWritableFile(const Containers::StringView& path);

		/** @brief Returns true if the file or directory is hidden */
		static bool IsHidden(const Containers::StringView& path);
		/** @brief Makes a file or directory hidden or not */
		static bool SetHidden(const Containers::StringView& path, bool hidden);

		/** @brief Creates a new directory */
		static bool CreateDirectories(const Containers::StringView& path);
		/** @brief Deletes an directory and all its content */
		static bool RemoveDirectoryRecursive(const Containers::StringView& path);
		/** @brief Deletes a file */
		static bool RemoveFile(const Containers::StringView& path);
		/** @brief Renames or moves a file or a directory */
		static bool Move(const Containers::StringView& oldPath, const Containers::StringView& newPath);
		/** @brief Copies a file */
		static bool Copy(const Containers::StringView& oldPath, const Containers::StringView& newPath, bool overwrite = true);

		/** @brief Returns the file size in bytes */
		static std::int64_t GetFileSize(const Containers::StringView& path);
		/** @brief Returns the last time the file or directory was modified */
		static FileDate GetLastModificationTime(const Containers::StringView& path);
		/** @brief Returns the last time the file or directory was accessed */
		static FileDate GetLastAccessTime(const Containers::StringView& path);

		/** @brief Returns the file or directory permissions in a mask */
		static Permission GetPermissions(const Containers::StringView& path);
		/** @brief Sets the file or directory permissions to those of the mask */
		static bool ChangePermissions(const Containers::StringView& path, Permission mode);
		/** @brief Adds the permissions in the mask to a file or a directory */
		static bool AddPermissions(const Containers::StringView& path, Permission mode);
		/** @brief Removes the permissions in the mask from a file or a directory */
		static bool RemovePermissions(const Containers::StringView& path, Permission mode);

		/** @brief Tries to open specified directory in operating system's file manager */
		static bool LaunchDirectoryAsync(const Containers::StringView& path);

#if defined(DEATH_TARGET_EMSCRIPTEN)
		/** @brief Mounts specified path to persistent file system (Emscripten only) */
		static void MountAsPersistent(const Containers::StringView& path);

		/** @brief Saves all changes to all persistent file systems (Emscripten only) */
		static void SyncToPersistent();
#endif

		/** @brief Opens file stream with specified access mode */
		static std::unique_ptr<Stream> Open(const Containers::String& path, FileAccessMode mode);

#if defined(DEATH_TARGET_UNIX) || (defined(DEATH_TARGET_WINDOWS) && !defined(DEATH_TARGET_WINDOWS_RT))
		/**
			@brief Memory-mapped file deleter

			@partialsupport Available only on @ref DEATH_TARGET_UNIX "Unix" and non-RT @ref DEATH_TARGET_WINDOWS "Windows" platforms.
		*/
		class MapDeleter
		{
#	if defined(DEATH_TARGET_UNIX)
		public:
			constexpr explicit MapDeleter() : _fd {} {}
			constexpr explicit MapDeleter(int fd) noexcept : _fd { fd } {}
			void operator()(const char* data, std::size_t size);
		private:
			int _fd;
#	elif defined(DEATH_TARGET_WINDOWS) && !defined(DEATH_TARGET_WINDOWS_RT)
		public:
			constexpr explicit MapDeleter() : _hFile {}, _hMap {} {}
			constexpr explicit MapDeleter(void* hFile, void* hMap) noexcept : _hFile { hFile }, _hMap { hMap } {}
			void operator()(const char* data, std::size_t size);
		private:
			void* _hFile;
			void* _hMap;
#	endif
		};

		static std::optional<Containers::Array<char, MapDeleter>> OpenAsMemoryMapped(const Containers::StringView& path, FileAccessMode mode);
#endif

		static std::unique_ptr<Stream> CreateFromMemory(std::uint8_t* bufferPtr, std::int32_t bufferSize);
		static std::unique_ptr<Stream> CreateFromMemory(const std::uint8_t* bufferPtr, std::int32_t bufferSize);

		/** @brief Returns application-specific writable directory for saving data */
		static const Containers::String& GetSavePath(const Containers::StringView& applicationName);

	private:
		FileSystem(const FileSystem&) = delete;
		FileSystem& operator=(const FileSystem&) = delete;
		
		static Containers::String _savePath;

		static void InitializeSavePath(const Containers::StringView& applicationName);
	};

	using fs = FileSystem;
}
