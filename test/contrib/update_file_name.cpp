#include "stdafx.h"
#include "update_file_name.h"

/************************************************************************/
/* author : wl
 * email  : lysgwl@163.com
 * date   : 2018.09.01 17:39
 */
/************************************************************************/

//update_file_data
update_file_data::update_file_data()
{
}

update_file_data::~update_file_data()
{

}

BOOL update_file_data::SetUpdateFileData(std::vector<UPDATE_FILEINFO*> &vecFileData, UPDATE_FILEDATA_CALLBACK_FUNC pfUpdateFileData)
{
	BOOL bRet = FALSE;
	
	UPDATE_FILEINFO *pFileInfo = NULL;
	UPDATE_FILEDATA *pFileData = NULL;

	UPDATE_FILEDATA stcUpdateFileData;
	std::vector<UPDATE_FILEINFO*>::iterator iterFileData;

	do 
	{
		memset(&stcUpdateFileData, 0x0, sizeof(UPDATE_FILEDATA));
		if (vecFileData.size() == 0 || pfUpdateFileData == NULL)
		{
			bRet = FALSE;
			break;
		}

		stcUpdateFileData.emUpdateStatus = STATE_UPDATEINPUTE_TYPE;
		if (!pfUpdateFileData(&stcUpdateFileData))
		{
			bRet = FALSE;
			break;
		}

		for (iterFileData = vecFileData.begin(); iterFileData!=vecFileData.end(); ++iterFileData)
		{
			pFileInfo = (UPDATE_FILEINFO *)(*iterFileData);
			if (pFileInfo == NULL)
			{
				continue;
			}

			pFileData = new UPDATE_FILEDATA;
			if (pFileData == NULL)
			{
				continue;
			}
			memset(pFileData, 0x0, sizeof(UPDATE_FILEDATA));

			memcpy(pFileData, &stcUpdateFileData, sizeof(UPDATE_FILEDATA));
			memcpy(&pFileData->stcFileInfo, pFileInfo, sizeof(UPDATE_FILEINFO));
			pFileData->pfUpdateFunc = pfUpdateFileData;

			m_vecFileData.push_back(pFileData);
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_data::GetUpdateFileData(std::vector<UPDATE_FILEDATA *> &vecFileData)
{
	BOOL bRet = FALSE;

	do 
	{
		if (m_vecFileData.size() == 0)
		{
			bRet = FALSE;
			break;
		}

		vecFileData.clear();
		vecFileData.assign(m_vecFileData.begin(), m_vecFileData.end());

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

void update_file_data::ClearFileData()
{
	BOOL bRet = FALSE;
	UPDATE_FILEDATA *pFileData = NULL;
	std::vector<UPDATE_FILEDATA *>::iterator iterFileData;

	do 
	{
		for (iterFileData=m_vecFileData.begin(); iterFileData!=m_vecFileData.end();)
		{
			pFileData = (UPDATE_FILEDATA *)(*iterFileData);
			if (pFileData != NULL)
			{
				delete pFileData;
				pFileData = NULL;

				iterFileData = m_vecFileData.erase(iterFileData);
			}
			else
			{
				++iterFileData;
			}
		}

		m_vecFileData.clear();
		bRet = TRUE;
	} while (FALSE);
}

//////////////////////////////////////////////////////////////////////////
//
update_file_func::update_file_func()
{

}

update_file_func::~update_file_func()
{

}

BOOL update_file_func::SetUpdateFileFunc(UPDATE_CONFIGTYPE emConfigType, update_file_data fileData)
{
	BOOL bRet = FALSE;

	do 
	{
		switch (emConfigType)
		{
		case CONFIG_ADDFILENAME_TYPE:
			{
				if (!SetAddFileName())
				{
					bRet = FALSE;
					break;
				}
			}
			break;

		case CONFIG_DATEFILENAME_TYPE:
			{
				if (!SetDateFileName())
				{
					bRet = FALSE;
					break;
				}
			}
			break;

		case CONFIG_DELFILENAME_TYPE:
			{
				if (!SetDelFileName())
				{
					bRet = FALSE;
					break;
				}
			}
			break;

		case CONFIG_EXTFILENAME_TYPE:
			{
				if (!SetExtFileName())
				{
					bRet = FALSE;
					break;
				}
			}
			break;

		case CONFIG_INDEXFILENAME_TYPE:
			{
				if (!SetIndexFileName())
				{
					bRet = FALSE;
					break;
				}
			}
			break;

		case CONFIG_REPLACEFILENAME_TYPE:
			{
				if (!SetReplaceFileName())
				{
					bRet = FALSE;
					break;
				}
			}
			break;

		default:
			break;
		}

		if (!bRet)
		{
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	return bRet;
}

BOOL update_file_func::SetAddFileName()
{
	return TRUE;
}

BOOL update_file_func::SetDateFileName()
{
	return TRUE;
}

BOOL update_file_func::SetDelFileName()
{
	return TRUE;
}

BOOL update_file_func::SetExtFileName()
{
	return TRUE;
}

BOOL update_file_func::SetIndexFileName()
{
	return TRUE;
}

BOOL update_file_func::SetReplaceFileName()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//update_file_name
update_file_name::update_file_name()
{
	m_bExit = FALSE;
	
	m_dwProcTimeOver = 500;
	m_dwCloseTimeOver = INFINITE;
	
	m_hThread = NULL;
	m_dwThreadID = 0;

	m_hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEndEvent   = CreateEvent(NULL, TRUE, FALSE, NULL);
}

update_file_name::~update_file_name()
{
	if (m_hStartEvent != NULL)
	{
		CloseHandle(m_hStartEvent);
		m_hStartEvent = NULL;
	}

	if (m_hEndEvent != NULL)
	{
		CloseHandle(m_hEndEvent);
		m_hEndEvent = NULL;
	}
}

DWORD update_file_name::UpdateFileThreadProc(LPVOID lpParam)
{
	update_file_name* pUpdateFileProc = (update_file_name*)lpParam;
	if(pUpdateFileProc != NULL)
	{
		pUpdateFileProc->UpdateFileInfo();
	}
	
	return 0;
}

void update_file_name::UpdateFileInfo()
{
	if (!UpdateFileName())
	{
	}
}

BOOL update_file_name::UpdateFileName()
{
	BOOL bRet = FALSE;

	std::vector<UPDATE_FILEDATA*> vecFileData;
	std::vector<UPDATE_FILEDATA*>::iterator iterFileData;

	do 
	{
		if (!m_fileData.GetUpdateFileData(vecFileData))
		{
			bRet = FALSE;
			break;
		}
		
	} while (FALSE);
}
//////////////////////////////////////////////////////////////////////////
//
BOOL update_file_name::CreateUpdateProc(update_file_data fileData)
{
	BOOL bRet = FALSE;

	do
	{
		if (WaitForSingleObject(m_hStartEvent, 0) != WAIT_OBJECT_0)
		{
			m_hThread = CreateThread(NULL, 0, UpdateFileThreadProc, (LPVOID)this, 0, &m_dwThreadID);
			if (m_hThread == NULL || m_hThread == INVALID_HANDLE_VALUE)
			{
				bRet = FALSE;
				break;
			}

			m_fileData = fileData;

			SetEvent(m_hStartEvent);
			ResetEvent(m_hEndEvent);
			
			bRet = TRUE;
		}	
	} while(FALSE);

	return bRet;
}

BOOL update_file_name::CloseUpdateProc()
{
	m_bExit = TRUE;
	WaitForSingleObject(m_hEndEvent, m_dwCloseTimeOver);
/*
	if (m_mapEnumInfo.size() > 0)
	{
		for (std::map<std::string, UPDATE_FILEINFO*>::iterator mapIter mapIter = m_mapEnumInfo.begin(); mapIter != m_mapEnumInfo.end();)
		{
			if (mapIter->second != NULL)
			{
				delete mapIter->second;
				mapIter->second = NULL;
			}

			mapIter = m_mapEnumInfo.erase(mapIter);
		}

		m_mapEnumInfo.clear();
	}
*/
	ResetEvent(m_hStartEvent);
	return TRUE;
}