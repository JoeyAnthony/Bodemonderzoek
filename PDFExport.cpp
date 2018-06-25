#include "stdafx.h"
#include "PDFExport.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

jmp_buf env; //TODO: move to class

void error_handler(HPDF_STATUS   error_no, HPDF_STATUS   detail_no, void *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	longjmp(env, 1);
}

PDFExport::PDFExport()
{
	pdf = HPDF_New(error_handler, NULL);
	if (!pdf) {
		throw "ERROR: cannot create pdf object.";
	}
	if (setjmp(env)) {
		HPDF_Free(pdf);
	}
	/* set compression mode */
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);

	font = HPDF_GetFont(pdf, "Helvetica", NULL);

}

void PDFExport::addPhoto(const std::string &fileName, const glm::vec2 &photoPosition, float angle/*, ChemicalStorage* pd*/)
{
	mutex.lock();
	isFinished = false;
	HPDF_Page page;
	page = HPDF_AddPage(pdf);

	HPDF_Page_SetWidth(page, 595.276f);
	HPDF_Page_SetHeight(page, 841.89f);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 20);
	HPDF_Page_MoveTextPos(page, 70, HPDF_Page_GetHeight(page) - 70);
	HPDF_Page_ShowText(page, "Foto xxxxx");
	HPDF_Page_EndText(page);

	float y;
	{
		HPDF_Image image =// HPDF_LoadPngImageFromFile(pdf, fileName.c_str());
			HPDF_LoadJpegImageFromFile(pdf, fileName.c_str());
		float ratio = HPDF_Image_GetHeight(image) / (float)HPDF_Image_GetWidth(image);
		float iw = HPDF_Page_GetWidth(page) - 140;
		float ih = iw * ratio;
		HPDF_Page_DrawImage(page, image, 70.0f, HPDF_Page_GetHeight(page) - 100 - ih, (float)iw, (float)ih);
		y = HPDF_Page_GetHeight(page) - 100 - ih;

		Photo photo;
		photo.fileName = fileName;
		photo.position = photoPosition;
		photo.angle = angle;
		photo.photo = image;
		photos.push_back(photo);
	}

	//{
	//	if (!minimap)
	//		minimap =// HPDF_LoadPngImageFromFile(pdf, ("data/virtueelpd/Photos/" + pd->startDateString + "/map.png").c_str());
	//		HPDF_LoadJpegImageFromFile(pdf, ("data/virtueelpd/Photos/" + pd->startDateString + "/map.jpg").c_str());

	//	float ratio = HPDF_Image_GetHeight(minimap) / (float)HPDF_Image_GetWidth(minimap);
	//	float iw = HPDF_Page_GetWidth(page) - 140;
	//	float ih = iw * ratio;

	//	float x1 = (photoPosition.x - pd->minimapCenter.x + pd->minimapScale) / (pd->minimapScale * 2) * iw;
	//	float y1 = (photoPosition.y - pd->minimapCenter.y + pd->minimapScale) / (pd->minimapScale * 2) * ih;

	//	float x2 = x1 + 10 * cos(angle + glm::pi<float>() / 2);
	//	float y2 = y1 + 10 * sin(angle + glm::pi<float>() / 2);

	//	HPDF_Page_DrawImage(page, minimap, 70.0f, y - ih, (float)iw, (float)ih);
	//	HPDF_Page_SetRGBStroke(page, 1, 0, 0);
	//	HPDF_Page_MoveTo(page, 70 + x1, y - y1);
	//	HPDF_Page_LineTo(page, 70 + x2, y - y2);
	//	HPDF_Page_MoveTo(page, 70 + x1, y - y1);
	//	HPDF_Page_Ellipse(page, 70 + x1, y - y1, 5, 5);
	//	HPDF_Page_Stroke(page);

	//}

	mutex.unlock();
}

void PDFExport::addRouteNode(const glm::vec2 &position, bool jump)
{
	route.push_back(glm::vec3(position, jump ? 1 : 0));
}

void PDFExport::save(const std::string & fileName)
{
	if (!photos.empty())
		HPDF_SaveToFile(pdf, fileName.c_str());
	else
		return;

	//if (!route.empty())
	//	generateRoutePage(pd);

}

void PDFExport::generateReport(std::string file)
{
	// Get filename from user
	frontPage();

	// Add student Info
	//generateRoutePage();
	// Add resultTable
	//generateResultPage(cs);

	HPDF_SaveToFile(pdf, "data/Bodemonderzoek/Reports/report.pdf");
}

//void PDFExport::addMapPage(ChemicalStorage* cs)
//{
//	// Add page
//	HPDF_Page page;
//	page = HPDF_AddPage(pdf);
//	HPDF_Page_SetWidth(page, 595.276f);
//	HPDF_Page_SetHeight(page, 841.89f);
//	HPDF_Page_BeginText(page);
//	HPDF_Page_SetFontAndSize(page, font, 20);
//	HPDF_Page_MoveTextPos(page, 70, HPDF_Page_GetHeight(page) - 70);
//	HPDF_Page_ShowText(page, "Map");
//	HPDF_Page_EndText(page);
//
//	// Add map screenshot
//	HPDF_Image map =// HPDF_LoadPngImageFromFile(pdf, ("data/ChemicalStorage/Photos/" + cs->startDateString + "/map.png").c_str());
//		HPDF_LoadJpegImageFromFile(pdf, ("data/ChemicalStorage/Photos/" + cs->startDateString + "/map.jpg").c_str());
//	float ratio = HPDF_Image_GetHeight(map) / (float)HPDF_Image_GetWidth(map);
//	float iw = HPDF_Page_GetWidth(page) - 140;
//	float ih = iw * ratio;
//	HPDF_Page_DrawImage(page, map, 70.0f, HPDF_Page_GetHeight(page) - 100 - ih, (float)iw, (float)ih);
//
//}

void PDFExport::converString(std::string in, std::wstring & out)
{
	std::wstring str1;
	for (int i = 0; i < in.length(); i++)
		str1 += (wchar_t)in[i];

	out = str1;
}

void  PDFExport::frontPage()
{
	HPDF_Page page;
	page = HPDF_AddPage(pdf);

	HPDF_Page_SetWidth(page, 595.276f);
	HPDF_Page_SetHeight(page, 841.89f);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 20);
	HPDF_Page_MoveTextPos(page, 70, HPDF_Page_GetHeight(page) - 70);
	HPDF_Page_ShowText(page, "Bodemonderzoek");

	HPDF_Page_SetFontAndSize(page, font, 12);
	HPDF_Page_MoveTextPos(page, 0, -20);
	HPDF_Page_MoveTextPos(page, 0, -600);
	//HPDF_Page_ShowText(page, "student name\t = ");
	HPDF_Page_ShowText(page, studentName.c_str());
	HPDF_Page_MoveTextPos(page, 0, -15);
	//HPDF_Page_ShowText(page, "student id\t = ");
	HPDF_Page_ShowText(page, studentID.c_str());
	HPDF_Page_MoveTextPos(page, 0, -15);

	//std::string text = cs->getScore();
	//HPDF_Page_ShowText(page, "found \t = ");
	//HPDF_Page_ShowText(page, text.c_str());
	//HPDF_Page_MoveTextPos(page, 0, -15);


	//text = cs->getRealScore() + " / " + cs->getMaxClickeble();
	//HPDF_Page_ShowText(page, "correct answered \t = ");
	//HPDF_Page_ShowText(page, text.c_str());
	//HPDF_Page_MoveTextPos(page, 0, -15);


	//text = cs->getTime();
	//HPDF_Page_ShowText(page, "end time\t = ");
	//HPDF_Page_ShowText(page, text.c_str());

	HPDF_Page_EndText(page);


	HPDF_Image mainImage = //HPDF_LoadPngImageFromFile(pdf, "data/ChemicalStorage/textures/mainPage.png");
		HPDF_LoadJpegImageFromFile(pdf, "data/Bodemonderzoek/textures/mainPage.jpg");
	float ratio = (float)HPDF_Image_GetWidth(mainImage) / HPDF_Image_GetHeight(mainImage);
	float ih = 550;
	float iw = ih * ratio;
	HPDF_Page_DrawImage(page, mainImage, HPDF_Page_GetWidth(page) / 2 - iw / 2, HPDF_Page_GetHeight(page) - 100 - ih, (float)iw, (float)ih);

	addAvansLogo(page);
}

void PDFExport::addAvansLogo(HPDF_Page page) {

	HPDF_Image avans = //HPDF_LoadPngImageFromFile(pdf, "data/ChemicalStorage/textures/Avans.png");
		HPDF_LoadJpegImageFromFile(pdf, "data/Bodemonderzoek/textures/Avans.jpg");
	float ratio = (float)HPDF_Image_GetHeight(avans) / HPDF_Image_GetWidth(avans);
	float iw = 200;
	float ih = iw * ratio;
	HPDF_Page_DrawImage(page, avans, HPDF_Page_GetWidth(page) - 20 - iw, HPDF_Page_GetHeight(page) - 10 - ih, (float)iw, (float)ih);
}

void PDFExport::generateRoutePage()
{
	HPDF_Page page;
	page = HPDF_AddPage(pdf);

	HPDF_Page_SetWidth(page, 595.276f);
	HPDF_Page_SetHeight(page, 841.89f);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 20);
	HPDF_Page_MoveTextPos(page, 70, HPDF_Page_GetHeight(page) - 70);
	HPDF_Page_ShowText(page, "Route");
	HPDF_Page_EndText(page);

	{
		HPDF_Image map =// HPDF_LoadPngImageFromFile(pdf, ("data/ChemicalStorage/Photos/" + cs->startDateString + "/map.png").c_str());
			HPDF_LoadJpegImageFromFile(pdf, "data/Bodemonderzoek/textures/WorldMap.jpg");
		float mapwidth = HPDF_Image_GetWidth(map);
		float mapheight = HPDF_Image_GetHeight(map);
		float ratio = mapheight / mapwidth;
		float iw = HPDF_Page_GetWidth(page) - 140;
		float ih = iw * ratio;
		float y = HPDF_Page_GetHeight(page) - 100;
		HPDF_Page_DrawImage(page, map, 70.0f, y - ih, (float)iw, (float)ih);

		bool first = true;
		glm::vec3 last = route[0];
		glm::vec2 pos;
		HPDF_Page_SetRGBStroke(page, 1, 0, 0);
		for (auto &r : route)
		{
			if (r.x == last.x && r.y == last.y && r.z == last.z)
				continue;

			if (r.z != last.z)
			{

				HPDF_Page_Stroke(page);
				if (r.z == 1)
				{
					auto gstate = HPDF_CreateExtGState(pdf);
					HPDF_ExtGState_SetAlphaFill(gstate, 0.5);
					HPDF_ExtGState_SetAlphaStroke(gstate, 0.5);
					HPDF_Page_SetExtGState(page, gstate);

					HPDF_Page_SetRGBStroke(page, 1, 0.5, 0.5);
				}
				else
				{
					auto gstate = HPDF_CreateExtGState(pdf);
					HPDF_ExtGState_SetAlphaFill(gstate, 1.0);
					HPDF_ExtGState_SetAlphaStroke(gstate, 1.0);
					HPDF_Page_SetExtGState(page, gstate);

					HPDF_Page_SetRGBStroke(page, 1, 0, 0);
				}
				HPDF_Page_MoveTo(page, 70 + pos.x, y - pos.y);
			}


			pos.x = (r.x - mapwidth / 8 )  * iw;
			pos.y = (r.y - mapheight / 8 )  * ih;
			if (first)
				HPDF_Page_MoveTo(page, 70 + pos.x, y - pos.y);
			else
				HPDF_Page_LineTo(page, 70 + pos.x, y - pos.y);
			first = false;
			last = r;
		}
		HPDF_Page_Stroke(page);
	}
}

//void PDFExport::generateResultPage(ChemicalStorage * cs)
//{
//	// Create a report page for every 5 objects
//	for (int i = 0; i < cs->clickableObjects.size(); i = i + 5)
//	{
//		// Add a Result page
//		HPDF_Page page;
//		page = HPDF_AddPage(pdf);
//		HPDF_Page_SetWidth(page, 595.276f);
//		HPDF_Page_SetHeight(page, 841.89f);
//		HPDF_Page_BeginText(page);
//		HPDF_Page_SetFontAndSize(page, font, 20);
//		HPDF_Page_MoveTextPos(page, 70, HPDF_Page_GetHeight(page) - 70);
//		HPDF_Page_ShowText(page, "Results");
//		HPDF_Page_MoveTextPos(page, 0, -20);
//
//		// Draw object report
//		for (int j = i; j < cs->clickableObjects.size() && j < (i + 5); j++)
//		{
//
//			// Add given object
//			HPDF_Page_SetFontAndSize(page, font, 13);
//			HPDF_Page_MoveTextPos(page, 0, -20);
//			//HPDF_Page_ShowText(page, "");
//			HPDF_Page_ShowText(page, cs->clickableObjects[j]->parent->name.c_str());
//
//			// Add given awnser
//			HPDF_Page_SetFontAndSize(page, font, 13);
//			HPDF_Page_MoveTextPos(page, 0, -20);
//			HPDF_Page_ShowText(page, "Gegeven antwoord: ");
//			HPDF_Page_ShowText(page, cs->clickableObjects[j]->getComponent<ChemicalObject>()->getAnwser().c_str());
//
//			// Add correct awnser
//			HPDF_Page_SetFontAndSize(page, font, 13);
//			HPDF_Page_MoveTextPos(page, 0, -20);
//			HPDF_Page_ShowText(page, "Correct antwoord: ");
//			HPDF_Page_ShowText(page, cs->clickableObjects[j]->getComponent<ChemicalObject>()->getError().c_str());
//			HPDF_Page_MoveTextPos(page, 0, -80);
//
//			// Add image
//
//		}
//
//		HPDF_Page_EndText(page);
//
//		for (int j = i; j < cs->clickableObjects.size() && j < (i + 5); j++)
//		{
//			//node->getComponent<ChemicalObject>()->setPicLocation
//			//cs->clickableObjects[j]->getComponent<ChemicalObject>()->getPicLocation(); 
//			AddImage(page, cs->clickableObjects[j]->getComponent<ChemicalObject>()->getPicLocation(), 100, 100, HPDF_Page_GetWidth(page) - 150, HPDF_Page_GetHeight(page) - ((j - i) * 140) - 170);
//			//HPDF_Page_GetWidth(page) - 20 - iw, HPDF_Page_GetHeight(page)
//		}
//
//	}
//
//
//}

void PDFExport::AddImage(HPDF_Page page, std::string path, int hight, int with, int locx, int locy) {

	try {
		HPDF_Image image = //HPDF_LoadPngImageFromFile(pdf, path.c_str());
			HPDF_LoadJpegImageFromFile(pdf, path.c_str());
		float iw = with;
		float ih = hight;
		HPDF_Page_DrawImage(page, image, locx, locy, (float)iw, (float)ih);
	}
	catch (const std::exception& e) {
		int i = 0;
		i++;
	}
}

//void PDFExport::generateDirectory(std::string startDate, std::string startTime)
//{
//	directory("data/Bodemonderzoek/Reports");
//	directory("data/Bodemonderzoek/photos");
//	directory(("data/Bodemonderzoek/Reports/" + startDate).c_str());
//	directory(("data/Bodemonderzoek/photos/" + startDate));
//	directory(("data/Bodemonderzoek/photos/" + startDate + "/" + startTime).c_str());
//}
//
//void PDFExport::directory(std::string path)
//{
//	std::wstring str;
//	for (int i = 0; i < path.length(); i++)
//	{
//		str += (wchar_t)path[i];
//	}
//	LPCWSTR temp1 = str.c_str();
//	CreateDirectory(temp1, nullptr);
//}