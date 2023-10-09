#include "Surface.h"
#include "Window.h"
#include "CustomUtils.h"
#include <algorithm>
#include <sstream>
#include <filesystem>

Surface::Surface(unsigned int width, unsigned int height)
{
	HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);
	if (FAILED(hr)) {
		throw Surface::Exception(__LINE__, __FILE__, "Error in initialize Scratch Image", hr);
	}
}

void Surface::Clear(Color fillValue) noexcept {
	const auto width = GetWidth();
	const auto height = GetHeight();
	const DirectX::Image& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++) {
		auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, fillValue);
	}
}

void Surface::PutPixel(unsigned int x, unsigned int y, Color c) noexcept(!IS_DEBUG) {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
}

Surface::Color Surface::GetPixel(unsigned int x, unsigned int y) const noexcept(!IS_DEBUG) {
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetHeight());

	auto& imgData = *scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
}

unsigned int Surface::GetWidth() const noexcept {
	return static_cast<unsigned int>(scratch.GetMetadata().width);
}

unsigned int Surface::GetHeight() const noexcept {
	return static_cast<unsigned int>(scratch.GetMetadata().height);
}

unsigned int Surface::GetBytePitch() const noexcept {
	return (unsigned int)scratch.GetImage(0, 0, 0)->rowPitch;
}

Surface::Color* Surface::GetBufferPtr() noexcept {
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

const Surface::Color* Surface::GetBufferPtr() const noexcept {
	// TODO why doing like this??
	return const_cast<Surface*>(this)->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept {
	return const_cast<Surface*>(this)->GetBufferPtr();
}

Surface Surface::FromFile(const std::string& name) {
	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(ToWide(name).c_str(), DirectX::WIC_FLAGS_NONE, nullptr, scratch);
	if (FAILED(hr)) {
		throw Surface::Exception(__LINE__, __FILE__, name, "Failed to load image", hr);
	}
	// TODO why not use GetMetadata()?
	if (scratch.GetImage(0, 0, 0)->format != format) {
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(*scratch.GetImage(0, 0, 0), format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, converted);
		if (FAILED(hr)) {
			throw Surface::Exception(__LINE__, __FILE__, name, "Failed to convert image", hr);
		}
		return Surface(std::move(converted));
	}
	return Surface(std::move(scratch));
}

void Surface::Save(const std::string& fileName) const {
	auto GetCodecId = [](const std::string& fileName) {
		const std::filesystem::path path(fileName);
		const auto ext = path.extension().string();
		if (ext == ".png") {
			return DirectX::WIC_CODEC_PNG;
		}
		else if (ext == ".jpg") {
			return DirectX::WIC_CODEC_JPEG;
		}
		else if (ext == ".bmp") {
			return DirectX::WIC_CODEC_BMP;
		}
		else if (ext == ".tiff") {
			return DirectX::WIC_CODEC_TIFF;
		}
		else if (ext == ".gif") {
			return DirectX::WIC_CODEC_GIF;
		}
		else if (ext == ".ico") {
			return DirectX::WIC_CODEC_ICO;
		}
		else if (ext == ".tif") {
			return DirectX::WIC_CODEC_TIFF;
		}
		else if (ext == ".wdp") {
			return DirectX::WIC_CODEC_WMP;
		}
		else {
			throw Surface::Exception(__LINE__, __FILE__, fileName, "Failed to find encoder for image format " + ext);
		}
	};

	HRESULT hr = DirectX::SaveToWICFile(*scratch.GetImage(0, 0, 0), DirectX::WIC_FLAGS_NONE, GetWICCodec(GetCodecId(fileName)), ToWide(fileName).c_str());
	if (FAILED(hr)) {
		throw Surface::Exception(__LINE__, __FILE__, fileName, "Failed to save image", hr);
	}
}

bool Surface::AlphaLoaded() const noexcept {
	return !scratch.IsAlphaAllOpaque();
}

Surface::Surface(DirectX::ScratchImage scratch) noexcept
	:
	scratch(std::move(scratch))
{}


Surface::Exception::Exception(int line, const char* file, std::optional<HRESULT> hr) noexcept 
	:
	BasicException(line, file),
	note("[Note]" + note)
{
	if (hr) {
		note = "[Error String] " + Window::Exception::TranslateErrorCode(*hr) + "\n" + note;
	}
}

Surface::Exception::Exception(int line, const char* file, std::string note, std::optional<HRESULT> hr) noexcept 
	:
	BasicException(line, file),
	note("[Note]" + note)
{}

// surface exception stuff
Surface::Exception::Exception(int line, const char* file, std::string fileName, std::string note_in, std::optional<HRESULT> hr) noexcept
	:
	BasicException(line, file),
	note("[File] " + fileName + "\n" + "[Note] " + note_in)
{
	if (hr) {
		note = "[Error String] " + Window::Exception::TranslateErrorCode(*hr) + note;
	}
}

const char* Surface::Exception::what() const noexcept {
	std::ostringstream oss;
	oss << BasicException::what() << std::endl
		<< "[Note] " << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::Exception::GetType() const noexcept {
	return "Graphics Exception";
}

const std::string& Surface::Exception::GetNote() const noexcept {
	return note;
}