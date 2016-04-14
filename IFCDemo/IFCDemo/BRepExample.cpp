// IFC SDK : IFC2X3 C++ Early Classes
// Copyright (C) 2009 CSTB
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full license is in Licence.txt file included with this
// distribution or is available at :
//     http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

#include <ifc2x3/SPFReader.h>
#include <ifc2x3/SPFWriter.h>
#include <ifc2x3/ExpressDataSet.h>
#include <ifc2x3/IfcProject.h>
#include <ifc2x3/IfcLocalPlacement.h>
#include <ifc2x3/IfcAxis2Placement.h>
#include <ifc2x3/IfcAxis2Placement2D.h>
#include <ifc2x3/IfcAxis2Placement3D.h>

#include <Step/CallBack.h>

#include <iostream>

#include "BrepBuilder.h"
#include "BrepReaderVisitor.h"
#include "comutil.h"


/*！@brief ANSI to UNICODE */
wchar_t* ANSI_to_UNICODE(const char* szAnsi)
{
	//预转换，得到所需空间的大小
	int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), NULL, 0);
	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	//转换
	::MultiByteToWideChar(CP_ACP, NULL, szAnsi, (int)strlen(szAnsi), wszString, wcsLen);
	//最后加上'\0'
	wszString[wcsLen] = '\0';
	return wszString;
}

/*！@brief UNICODE to UTF8 */
const char* UNICODE_to_UTF8(wchar_t* wszString)
{
	//预转换，得到所需空间的大小，这次用的函数和上面名字相反
	int u8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);
	//同上，分配空间要给'\0'留个空间
	//UTF8虽然是Unicode的压缩形式，但也是多字节字符串，所以可以以char的形式保存
	char* szU8 = new char[u8Len + 1];
	//转换
	//unicode版对应的strlen是wcslen
	::WideCharToMultiByte(CP_UTF8, NULL, wszString, (int)wcslen(wszString), szU8, u8Len, NULL, NULL);
	//最后加上'\0'
	szU8[u8Len] = '\0';
	return szU8;
}

/*！@brief UTF8 to UNICODE */
wchar_t* KS_UTF8_TO_UNICODE(const char* szUTF8)
{
	//UTF8 to Unicode
	//预转换，得到所需空间的大小
	int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), NULL, 0);
	//分配空间要给'\0'留个空间，MultiByteToWideChar不会给'\0'空间
	wchar_t* wszString = new wchar_t[wcsLen + 1];
	//转换
	::MultiByteToWideChar(CP_UTF8, NULL, szUTF8, (int)strlen(szUTF8), wszString, wcsLen);
	//最后加上'\0'
	wszString[wcsLen] = '\0';
	return wszString;
}

/*！@brief UNICODE to UTF8 */
const char* KS_UNICODE_to_ANSI (wchar_t* wszString)
{
	// unicode to ansi
	//预转换，得到所需空间的大小，这次用的函数和上面名字相反
	int ansiLen = ::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), NULL, 0, NULL, NULL);
	//同上，分配空间要给'\0'留个空间
	char* szAnsi = new char[ansiLen + 1];
	//转换
	//unicode版对应的strlen是wcslen
	::WideCharToMultiByte(CP_ACP, NULL, wszString, (int)wcslen(wszString), szAnsi, ansiLen, NULL, NULL);
	//最后加上'\0'
	szAnsi[ansiLen] = '\0';
	return szAnsi;
}



class ConsoleCallBack : public Step::CallBack
{
public:
    ConsoleCallBack() : _max(1) {}
    virtual void setMaximum(size_t max) { _max = max; }
    virtual void setProgress(size_t progress) { std::cerr << double(progress)/double(_max)*100.0 << "%" << std::endl; }
protected:
    size_t _max;
};


int main(int argc, char **argv)
{
    std::cout << "Simple read Brep geometry example of Ifc2x3 SDK" << std::endl;

    /*if (argc < 2)
    {
        std::cout << "no ifc file to read in command line ." << std::endl;
        return 1;
    }*/

    bool inMemory = true;
    /*if (argc < 4)
    {
        inMemory = false;
    }*/

    // ** open, load, close the file
    std::ifstream ifcFile;
    //ifcFile.open(argv[1]);
	ifcFile.open("E:/Share/IFC/OneWall.ifc");
    ifc2x3::SPFReader reader;
    ConsoleCallBack cb;
    reader.setCallBack(&cb);
    if ( ifcFile.is_open() )
    {
        std::cout << "reading file ..." << std::endl;
    }
    else
    {
        std::cout << "ERROR: failed to open <" << argv[1] << ">" << std::endl;
        return 1;
    }

    // get length of file
    ifcFile.seekg (0, ifcFile.end);
    std::ifstream::pos_type length = ifcFile.tellg();
    ifcFile.seekg (0, ifcFile.beg);

    bool result = reader.read(ifcFile, inMemory ? length : (std::ifstream::pos_type)0);
    ifcFile.close();

    if (result)
        std::cout << "OK!!" << std::endl;
    else
    {
        std::cout << "Ho no, there is a PROBLEM!!" << std::endl;
        std::vector<std::string> errors = reader.errors();
        std::vector<std::string>::iterator it = errors.begin();
        while(it != errors.end())
        {
            std::cout << *it << std::endl;
            ++it;
        }

        return 1;
    }

    // ** get the model
    ifc2x3::ExpressDataSet * expressDataSet = dynamic_cast<ifc2x3::ExpressDataSet*>(reader.getExpressDataSet());
	
    if (expressDataSet == NULL)
    {
        std::cout << "Ho no ... there is no ExpressDataSet." << std::endl;
        return (2);
    }

    // ** Get buildingElement
	BRepBuilder brepBuilder;
	BrepReaderVisitor visitor(&brepBuilder);
	
	Step::RefLinkedList< ifc2x3::IfcWallStandardCase >::iterator wallIt = expressDataSet->getAllIfcWallStandardCase().begin();
    for (; wallIt != expressDataSet->getAllIfcWallStandardCase().end(); ++wallIt)
    {
        wallIt->acceptVisitor(&visitor);
		ifc2x3::IfcGloballyUniqueId guid = wallIt->getGlobalId();	// 全局ID
		ifc2x3::IfcOwnerHistory* owner = wallIt->getOwnerHistory();	// 拥有者信息
		ifc2x3::IfcPersonAndOrganization* ownerUser = owner->getOwningUser();
		ifc2x3::IfcPerson* person = ownerUser->getThePerson();
		ifc2x3::IfcLabel personName = person->getGivenName();		// 拥有者名字
		std::string szWallName = KS_UNICODE_to_ANSI((wchar_t*)wallIt->getName().c_str());		// 墙的名字
		std::string szObjName = KS_UNICODE_to_ANSI((wchar_t*)wallIt->getObjectType().c_str());	// 物体名字
		ifc2x3::IfcIdentifier tag = wallIt->getTag();	// Tag
		ifc2x3::IfcLocalPlacement* objPlacement = (ifc2x3::IfcLocalPlacement*)wallIt->getObjectPlacement();
		std::string currentTypeName = objPlacement->getRelativePlacement()->currentTypeName();
		ifc2x3::IfcAxis2Placement3D * axis2Placement3D = objPlacement->getRelativePlacement()->getIfcAxis2Placement3D();
		ifc2x3::IfcCartesianPoint* localPoint = axis2Placement3D->getLocation();
		ifc2x3::List_IfcLengthMeasure_1_3 coor3d = localPoint->getCoordinates();	// 位置
		ifc2x3::IfcDirection* axis = axis2Placement3D->getAxis();
		ifc2x3::List_Real_2_3 axis3d = axis->getDirectionRatios();
		ifc2x3::IfcDirection* direction = axis2Placement3D->getRefDirection();
		ifc2x3::List_Real_2_3 direction3d = direction->getDirectionRatios();
		ifc2x3::IfcProductRepresentation* mdlRep = wallIt->getRepresentation();
		ifc2x3::List_IfcRepresentation_1_n repList = mdlRep->getRepresentations();
		for (auto &it:repList){
			ifc2x3::IfcLabel label = it->getRepresentationType();
			ifc2x3::Set_IfcRepresentationItem_1_n items = it->getItems();
			if(L"SweptSolid" == label){
				for (auto &item : items){
					Step::RefPtr<ifc2x3::IfcExtrudedAreaSolid> shapeRepresentation = item.get();
					ifc2x3::IfcPositiveLengthMeasure depth = shapeRepresentation->getDepth();
					ifc2x3::IfcRectangleProfileDef*  rect = (ifc2x3::IfcRectangleProfileDef*)shapeRepresentation->getSweptArea();
					ifc2x3::IfcPositiveLengthMeasure fx = rect->getXDim();
					ifc2x3::IfcPositiveLengthMeasure fy = rect->getYDim();
					ifc2x3::IfcAxis2Placement3D* pos = shapeRepresentation->getPosition();
					break;
				}
			}
		}
		break;
    }
	
    return 0;
}
