#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <glm/glm.hpp>
#include <hpdf.h>
#include <zlib.h>

class PDFExport
{
	class Photo
	{
	public:
		std::string fileName;
		glm::vec2 position;
		float angle;
		HPDF_Image photo;
	};
	HPDF_Doc pdf;
	HPDF_Font font;
	HPDF_Image minimap = nullptr;

	std::mutex mutex;
	std::atomic<bool> isFinished = true;

	std::vector<Photo> photos;
	std::vector<glm::vec2> route;
public:
	PDFExport();

	void addPhoto(const std::string &fileName, const glm::vec2 &photoPosition, float angle/*, ChemicalStorage* pd*/);
	void addRouteNode(const glm::vec2 &position);
	void save(const std::string &fileName);

	void generateReport(std::string file);
	void frontPage();
	//void addMapPage(ChemicalStorage* cs);
	void addAvansLogo(HPDF_Page page);
	static void converString(std::string in, std::wstring & out);

	//void generateDirectory(std::string startDate, std::string startTime);
	//void directory(std::string path);
	void AddImage(HPDF_Page page, std::string path, int hight, int with, int locx, int locy);


	std::string studentName = "insert name";
	std::string studentID = "0000000";

private:
	void generateRoutePage();
	//void generateResultPage(ChemicalStorage* cs);
};

