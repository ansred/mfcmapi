#include <StdAfx.h>
#include <Interpret/SmartView/GlobalObjectId.h>
#include <Interpret/InterpretProp.h>
#include <Interpret/ExtraPropTags.h>

namespace smartview
{
	GlobalObjectId::GlobalObjectId() {}

	// clang-format off
	static const BYTE s_rgbSPlus[] =
	{
		0x04, 0x00, 0x00, 0x00,
		0x82, 0x00, 0xE0, 0x00,
		0x74, 0xC5, 0xB7, 0x10,
		0x1A, 0x82, 0xE0, 0x08,
	};
	// clang-format on

	void GlobalObjectId::Parse()
	{
		addHeader(L"Global Object ID:\r\n");
		addHeader(L"Byte Array ID = ");

		m_Id = m_Parser.GetBlockBYTES(16);
		addBlock(m_Id, strings::BinToHexString(m_Id.getData(), true));

		if (equal(m_Id.getData().begin(), m_Id.getData().end(), s_rgbSPlus))
		{
			addHeader(L" = s_rgbSPlus\r\n");
		}
		else
		{
			addHeader(L" = Unknown GUID\r\n");
		}

		const auto b1 = m_Parser.GetBlock<BYTE>();
		const auto b2 = m_Parser.GetBlock<BYTE>();
		m_Year.setData(static_cast<WORD>(b1.getData() << 8 | b2.getData()));
		m_Year.setOffset(b1.getOffset());
		m_Year.setSize(b1.getSize() + b2.getSize());
		addBlock(m_Year, strings::formatmessage(L"Year: 0x%1!04X! = %1!d!\r\n", m_Year.getData()));

		m_Month = m_Parser.GetBlock<BYTE>();
		const auto szFlags = interpretprop::InterpretFlags(flagGlobalObjectIdMonth, m_Month.getData());
		addBlock(
			m_Month,
			strings::formatmessage(L"Month: 0x%1!02X! = %1!d! = %2!ws!\r\n", m_Month.getData(), szFlags.c_str()));

		m_Day = m_Parser.GetBlock<BYTE>();
		addBlock(m_Day, strings::formatmessage(L"Day: 0x%1!02X! = %1!d!\r\n", m_Day.getData()));

		m_CreationTime = m_Parser.GetBlock<FILETIME>();
		std::wstring propString;
		std::wstring altPropString;
		strings::FileTimeToString(m_CreationTime.getData(), propString, altPropString);
		addBlock(
			m_CreationTime,
			strings::formatmessage(
				L"Creation Time = 0x%1!08X!:0x%2!08X! = %3!ws!\r\n",
				m_CreationTime.getData().dwHighDateTime,
				m_CreationTime.getData().dwLowDateTime,
				propString.c_str()));

		m_X = m_Parser.GetBlock<LARGE_INTEGER>();
		addBlock(
			m_X, strings::formatmessage(L"X: 0x%1!08X!:0x%2!08X!\r\n", m_X.getData().HighPart, m_X.getData().LowPart));

		m_dwSize = m_Parser.GetBlock<DWORD>();
		addBlock(m_dwSize, strings::formatmessage(L"Size: 0x%1!02X! = %1!d!\r\n", m_dwSize.getData()));

		m_lpData = m_Parser.GetBlockBYTES(m_dwSize.getData(), _MaxBytes);
		if (m_lpData.getData().size())
		{
			addHeader(L"Data = ");
			addBlock(m_lpData, strings::BinToHexString(m_lpData.getData(), true));
		}
	}
}