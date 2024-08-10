#pragma once

#include <string>
#include <array>
#include <functional>
#include <unordered_map>


#include "svgshapes.h"
#include "svgpath.h"
#include "svgcss.h"


//
// SVG Supports embedded fonts.  This feature is little used, and obsolete
// as web fonts, and truetype fonts are the norm.  The support here is experimental
// as there's no real way of adding this information in this form into blend2d
//

namespace waavs {
	enum GLYPH_ORIENTATION {
		GLYPH_ORIENTATION_HORIZONTAL=1
		, GLYPH_ORIENTATION_VERTICAL=2
		, GLYPH_ORIENTATION_BOTH=3
	};
	
	enum GLYPH_ARABIC_FORM {
		GLYPH_ARABIC_FORM_ISOLATED = 0
		, GLYPH_ARABIC_FORM_INITIAL = 1
		, GLYPH_ARABIC_FORM_MEDIAL = 2
		, GLYPH_ARABIC_FORM_TERMINAL = 3
	};
	
	//====================================
	// SVGFont
	// Support the 'font' element
	//====================================
	struct SVGFontNode : public SVGGraphicsElement
	{
		static void registerFactory()
		{
			gSVGGraphicsElementCreation["font"] = [](IAmGroot* groot, XmlElementIterator& iter) {
				auto node = std::make_shared<SVGFontNode>(groot);
				node->loadFromXmlIterator(iter, groot);
				node->visible(false);

				return node;
			};
		}

		// id
		double fHorizAdvX;
		double fHorizOriginX;
		double fHorizOriginY;
		double fVertAdvY;
		double fVertOriginX;
		double fVertOriginY;

		SVGFontNode(IAmGroot* aroot)
			: SVGGraphicsElement(aroot)
		{
			isStructural(false);
		}

		void loadSelfFromXmlElement(const XmlElement& elem, IAmGroot* groot) override
		{
			SVGGraphicsElement::loadSelfFromXmlElement(elem, groot);


			// get horiz-adv-x as a numeric value if it exists
			auto horizAdvX = getAttribute("horiz-adv-x");
			auto horizOriginX = getAttribute("horiz-origin-x");
			auto horizOriginY = getAttribute("horiz-origin-y");
			auto vertAdvY = getAttribute("vert-adv-y");
			auto vertOriginX = getAttribute("vert-origin-x");
			auto vertOriginY = getAttribute("vert-origin-y");


			if (horizAdvX)
				fHorizAdvX = toNumber(horizAdvX);
			if (horizOriginX)
				fHorizOriginX = toNumber(horizOriginX);
			if (horizOriginY)
				fHorizOriginY = toNumber(horizOriginY);
			if (vertAdvY)
				fVertAdvY = toNumber(vertAdvY);
			if (vertOriginX)
				fVertOriginX = toNumber(vertOriginX);
			if (vertOriginY)
				fVertOriginY = toNumber(vertOriginY);


			needsBinding(true);
		}

	};
	
	//====================================
	// SVGFontFaceNode
	// Has both singular and compound forms
	//====================================
	struct SVGFontFaceNode : public SVGGraphicsElement
	{
		static void registerSingularNode()
		{
			gShapeCreationMap["font-face"] = [](IAmGroot* groot, const XmlElement& elem) {
				auto node = std::make_shared<SVGFontFaceNode>(groot);
				node->loadFromXmlElement(elem, groot);
				node->visible(false);

				return node;
			};
		}

		static void registerFactory()
		{
			gSVGGraphicsElementCreation["font-face"] = [](IAmGroot* groot, XmlElementIterator& iter) {
				auto node = std::make_shared<SVGFontFaceNode>(groot);
				node->loadFromXmlIterator(iter, groot);
				node->visible(false);

				return node;
			};

			registerSingularNode();
		}

		std::string fFontFamily;
		std::string fFontStyle;
		std::string fFontVariant;
		std::string fFontWeight;
		std::string fFontStretch;
		std::string fFontSize;
		std::string fUnicodeRange;
		std::string fUnitsPerEm;
		std::string fPanose1;
		std::string fStemV;
		std::string fStemH;
		std::string fSlope;
		std::string fCapHeight;
		std::string fXHeight;
		std::string fAccentHeight;
		std::string fAscent;
		std::string fDescent;
		std::string fWidths;
		std::string fBbox;
		std::string fIdeographic;
		std::string fAlphabetic;
		std::string fMathematical;
		std::string fHanging;
		std::string fVideographic;
		std::string fValphabetic;
		std::string fVmathematical;
		std::string fVhanging;
		std::string fUnderlinePosition;
		std::string fUnderlineThickness;
		std::string fStrikethroughPosition;
		std::string fStrikethroughThickness;
		std::string fOverlinePosition;
		std::string fOverlineThickness;

		SVGFontFaceNode(IAmGroot* iMap) :SVGGraphicsElement(iMap) {}

		void loadSelfFromXmlElement(const XmlElement& elem, IAmGroot* groot) override
		{
			SVGGraphicsElement::loadSelfFromXmlElement(elem, groot);

			auto fontFamily = getAttribute("font-family");
			auto fontStyle = getAttribute("font-style");
			auto fontVariant = getAttribute("font-variant");
			auto fontWeight = getAttribute("font-weight");
			auto fontStretch = getAttribute("font-stretch");
			auto fontSize = getAttribute("font-size");
			auto unicodeRange = getAttribute("unicode-range");
			auto unitsPerEm = getAttribute("units-per-em");
		}
	};

	
	//====================================
	// SVGMissingGlyphNode
	// Can be a singular node, or a compound
	// node with an embedded path
	// Really glyphs act like symbols
	//====================================
	struct SVGMissingGlyphNode : public SVGGraphicsElement
	{
		static void registerSingularNode()
		{
			gShapeCreationMap["missing-glyph"] = [](IAmGroot* groot, const XmlElement& elem) {
				auto node = std::make_shared<SVGMissingGlyphNode>(groot);
				node->loadFromXmlElement(elem, groot);

				return node;
			};
		}

		static void registerFactory()
		{
			gSVGGraphicsElementCreation["missing-glyph"] = [](IAmGroot* groot, XmlElementIterator& iter) {
				auto node = std::make_shared<SVGMissingGlyphNode>(groot);
				node->loadFromXmlIterator(iter, groot);

				return node;
			};
			
			registerSingularNode();
		}
		
		double fHorizAdvX{ 0 };
		double fVertAdvY{ 0 };
		double fVertOriginX{ 0 };
		double fVertOriginY{ 0 };
		BLPath fPath{};
		
		SVGMissingGlyphNode(IAmGroot* aroot)
			: SVGGraphicsElement(aroot)
		{
		}

		void loadSelfFromXmlElement(const XmlElement& elem, IAmGroot* groot) override
		{
			SVGGraphicsElement::loadSelfFromXmlElement(elem, groot);

			auto horizAdvX = getAttribute("horiz-adv-x");
			auto vertAdvY = getAttribute("vert-adv-y");
			auto vertOriginX = getAttribute("vert-origin-x");
			auto vertOriginY = getAttribute("vert-origin-y");
			
			if (horizAdvX)
				fHorizAdvX = toNumber(horizAdvX);
			if (vertAdvY)
				fVertAdvY = toNumber(vertAdvY);
			if (vertOriginX)
				fVertOriginX = toNumber(vertOriginX);
			if (vertOriginY)
				fVertOriginY = toNumber(vertOriginY);
			

			auto d = getAttribute("d");
			if (!d)
				return;

			auto success = blpathparser::parsePath(d, fPath);
			fPath.shrink();

			needsBinding(false);
		}
	};
	
	//====================================
	// SVGGlyph
	// Support the 'glyph' found in the 'font' element
	//====================================
	struct SVGGlyphNode : public SVGGeometryElement
	{
		static void registerFactory() {
			gShapeCreationMap["glyph"] = [](IAmGroot* groot, const XmlElement& elem) {
				auto node = std::make_shared<SVGGlyphNode>(groot);
				node->loadFromXmlElement(elem, groot);
				
				return node;
			};
		}

		uint64_t fUnicode{ 0 };
		double fHorizAdvX{ 0 };
		double fVertAdvY{ 0 };
		double fVertOriginX{ 0 };
		double fVertOriginY{ 0 };
		
		std::string fGlyphName{};
		GLYPH_ORIENTATION fOrientation{ GLYPH_ORIENTATION_BOTH };
		GLYPH_ARABIC_FORM arabicForm{ GLYPH_ARABIC_FORM_ISOLATED };
		ByteSpan fLang{};

		SVGGlyphNode(IAmGroot* iMap) :SVGGeometryElement(iMap) {}

		void loadSelfFromXmlElement(const XmlElement& elem, IAmGroot* groot) override
		{
			SVGGeometryElement::loadSelfFromXmlElement(elem, groot);

			// load unicode property
			// just store the raw ByteSpan
			// and let the caller parse it
			auto uni = getAttribute("unicode");
			auto horizAdvX = getAttribute("horiz-adv-x");
			auto vertAdvY = getAttribute("vert-adv-y");
			auto vertOriginX = getAttribute("vert-origin-x");
			auto vertOriginY = getAttribute("vert-origin-y");
			
			if (horizAdvX)
				fHorizAdvX = toNumber(horizAdvX);
			if (vertAdvY)
				fVertAdvY = toNumber(vertAdvY);
			if (vertOriginX)
				fVertOriginX = toNumber(vertOriginX);
			if (vertOriginY)
				fVertOriginY = toNumber(vertOriginY);
			
			auto d = getAttribute("d");
			if (!d)
				return;

			auto success = blpathparser::parsePath(d, fPath);
			fPath.shrink();

			needsBinding(false);
		}

	};

	//====================================
	// SVGFontFaceNode
	// Has both singular and compound forms
	//====================================
	struct SVGFontFaceSrcNode : public SVGGraphicsElement
	{
		static void registerSingularNode()
		{
			gShapeCreationMap["font-face-src"] = [](IAmGroot* groot, const XmlElement& elem) {
				auto node = std::make_shared<SVGFontFaceSrcNode>(groot);
				node->loadFromXmlElement(elem, groot);

				return node;
			};
		}

		static void registerFactory()
		{
			gSVGGraphicsElementCreation["font-face-src"] = [](IAmGroot* groot, XmlElementIterator& iter) {
				auto node = std::make_shared<SVGFontFaceSrcNode>(groot);
				node->loadFromXmlIterator(iter, groot);

				return node;
			};

			registerSingularNode();
		}


		SVGFontFaceSrcNode(IAmGroot* iMap) :SVGGraphicsElement(iMap) {}

	};
	
	//=========================================================
	// SVGFontFaceNameNode
	//=========================================================
	struct SVGFontFaceNameNode : public SVGVisualNode
	{
		static void registerFactory() {
			gShapeCreationMap["font-face-name"] = [](IAmGroot* groot, const XmlElement& elem) {
				auto node = std::make_shared<SVGFontFaceNameNode>(groot);
				node->loadFromXmlElement(elem, groot);
				return node;
			};
		}

		std::string fFaceName{};

		SVGFontFaceNameNode(IAmGroot* iMap) :SVGVisualNode(iMap) { visible(false); }

		const std::string& faceName() const {
			return fFaceName;
		}
		
		void loadSelfFromXmlElement(const XmlElement& elem, IAmGroot* groot) override
		{
			auto aname = getAttribute("name");
			if (aname)
				fFaceName = toString(aname);
		}


	};
	

}