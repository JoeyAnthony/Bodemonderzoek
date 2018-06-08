#pragma once
#include <VrLib\tien\Tien.h>

class OcclusionQuery 
{
private:
	GLuint type = GL_SAMPLES_PASSED;
	GLuint queryID = 0;
	GLint renderedSamples = 0;
	bool hasResults = false;

public:
	/*
	*start querying
	*/
	void beginQuery();
	/*
	*stop querying
	*/
	void endQuery();

	/*
	* Older way of checking results
	* returns true if there are results avaiable
	*/
	bool resultsAvailable();

	/*
	* Checks if there are results, results are saved in renderedsamples.
	* returns true when there are NEW results
	*/
	bool getResult();

	/*
	*Returns amount of samples that are rendered
	*/
	const int lastRenderedSamples();


	OcclusionQuery();
	~OcclusionQuery();
};

