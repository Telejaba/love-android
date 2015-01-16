/**
 * Copyright (c) 2006-2015 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "CompressedData.h"

#include "ddsHandler.h"

namespace love
{
namespace image
{
namespace magpie
{

CompressedData::CompressedData(std::list<CompressedFormatHandler *> formats, love::filesystem::FileData *filedata)
	: formatHandlers(formats)
{
	for (CompressedFormatHandler *handler : formatHandlers)
		handler->retain();

	load(filedata);
}

CompressedData::~CompressedData()
{
	delete[] data;

	for (CompressedFormatHandler *handler : formatHandlers)
		handler->release();
}

void CompressedData::load(love::filesystem::FileData *filedata)
{
	CompressedFormatHandler *parser = nullptr;

	for (CompressedFormatHandler *handler : formatHandlers)
	{
		if (handler->canParse(filedata))
		{
			parser = handler;
			break;
		}
	}

	if (parser == nullptr)
		throw love::Exception("Could not parse compressed data: Unknown format.");

	// SubImage vector will be populated by a parser.
	std::vector<SubImage> parsedimages;
	Format texformat = FORMAT_UNKNOWN;

	size_t newdata_size = 0;
	uint8 *newdata = parser->parse(filedata, parsedimages, newdata_size, texformat);

	if (newdata == nullptr)
		throw love::Exception("Could not parse compressed data.");

	if (texformat == FORMAT_UNKNOWN)
	{
		delete[] newdata;
		throw love::Exception("Could not parse compressed data: Unknown format.");
	}

	if (parsedimages.size() == 0 || newdata_size == 0)
	{
		delete[] newdata;
		throw love::Exception("Could not parse compressed data: No valid data?");
	}

	// Make sure to clean up any previously loaded data.
	delete[] data;

	data = newdata;
	dataSize = newdata_size;

	dataImages = parsedimages;
	format = texformat;
}

} // magpie
} // image
} // love
