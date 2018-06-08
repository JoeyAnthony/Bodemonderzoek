#include "stdafx.h"
#include "OcclusionQuery.h"



void OcclusionQuery::beginQuery()
{
	hasResults = false;
	glBeginQuery(type, queryID);
}

void OcclusionQuery::endQuery()
{
	glEndQuery(type);
}

bool OcclusionQuery::resultsAvailable()
{
	GLint param;
	glGetQueryObjectiv(queryID, GL_QUERY_RESULT_AVAILABLE, &param);
	if (param)
		hasResults = true;

	//TODO test if param is true or false
	return param;
}


bool OcclusionQuery::getResult()
{
	int oldsamples = renderedSamples;
	glGetQueryObjectiv(queryID, GL_QUERY_RESULT_NO_WAIT, &renderedSamples);
	if (oldsamples == renderedSamples)
		return false;
	return true;
}

const int OcclusionQuery::lastRenderedSamples()
{
	return renderedSamples;
}



OcclusionQuery::OcclusionQuery()
{
	glGenQueries(1, &queryID);
}

OcclusionQuery::~OcclusionQuery()
{
}
