#include "DefaultText.h"
#include "Graphics.h"
#include "VertexTypes.h"

namespace OBALFramework
{
  DefaultText::DefaultText()
  {
    Text = "DefaultText";
    Count = -1;
    TextRect.left = 0;
    TextRect.right = 800;
    TextRect.top = 0;
    TextRect.bottom = 800;
    TextFormat = DT_LEFT;
    TextColor = D3DXCOLOR(255, 255, 255, 255);
    Initialize();
  }
  DefaultText::DefaultText(char * txt, int left, int right, int top, int bottom, UINT format, D3DXCOLOR color)
  {
    Text = txt;
    Count = -1;   //When DrawTextA is called, the text is assumed to be a null-terminated string if count is -1
    TextRect.left = left;
    TextRect.right = right;
    TextRect.top = top;
    TextRect.bottom = bottom;
    TextFormat = format;
    TextColor = color;
    Initialize();
  }
  DefaultText::~DefaultText()
  {
    GRAPHICS->DirectTextVec.erase(this);
  }

  void DefaultText::Initialize()
  {
    GRAPHICS->DirectTextVec.push_back(this);
  }


  void DefaultText::SetText(char * newtxt)
  { Text = newtxt; }
  void DefaultText::AppendText(char * newtxt)
  { Text += newtxt; }

  void DefaultText::SetFormat_TopLeft()
  { TextFormat = DT_LEFT; }
  void DefaultText::SetFormat_Center()
  { TextFormat = DT_CENTER; }
  void DefaultText::SetFormat_Right()
  { TextFormat = DT_RIGHT; }


  void DefaultText::Draw(LPD3DXFONT * pFont)
  {
    (*pFont)->DrawTextA(NULL, Text.c_str(), Count, &TextRect, TextFormat, TextColor);
  }

}