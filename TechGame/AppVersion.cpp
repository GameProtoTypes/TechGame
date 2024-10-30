#include "AppVersion.h"



AppVersion::AppVersion(Context* context) : Object(context)
{

}

AppVersion::~AppVersion()
{

}

void AppVersion::RegisterObject(Context* context)
{
	context->RegisterFactory<AppVersion>();
}
