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

class ConsoleCallBack : public Step::CallBack
{
public:
    ConsoleCallBack() : _max(1) {}
    virtual void setMaximum(size_t max) { _max = max; }
    virtual void setProgress(size_t progress) { std::cerr << double(progress)/double(_max)*100.0 << "%" << std::endl; }
protected:
    size_t _max;
};



int main(int argc, char *argv[])
{
	std::ifstream ifcFile;
	//ifcFile.open(argv[1]);
	ifcFile.open("e:/4.ifc");
	ifc2x3::SPFReader reader;
	ConsoleCallBack cb;
	reader.setCallBack(&cb);
	bool inMemory = true;
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
	Step::MapOfEntities entitys = expressDataSet->getAll();
	int nSize = entitys.size();
	for (auto it : entitys)
	{
		it.first;
		const Step::ClassType  classType = it.second->getType();
		Step::Id id = it.second->getKey();
		Step::SPFData* data = it.second->getArgs();
		//int argCount = data->argc();
		//for(int i = 0; i < argCount; i++){
		//	//char msg[255] = {0};
		//	
		//	printf("ID: %d ArgIndex %d Content %s \n",id, i, data[i]);
		//	//std::cout << msg << std::endl;
		//}
			
	}
	getchar();
	return 0;
}