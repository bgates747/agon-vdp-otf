// di_horiz_line.cpp - Function definitions for drawing horizontal lines
//
// A horizontal line is N pixels wide and 1 pixel high.
//
// Copyright (c) 2023 Curtis Whitley
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// 

#include "di_horiz_line.h"

DiHorizontalLine::DiHorizontalLine(uint16_t flags) : DiPrimitive(flags) {
  m_flags |= PRIM_FLAGS_X;
}

void DiHorizontalLine::make_line(int32_t x, int32_t y, uint32_t width, uint8_t color) {
  m_opaqueness = DiPrimitive::normal_alpha_to_opaqueness(color);
  m_rel_x = x;
  m_rel_y = y;
  m_width = width;
  m_height = 1;
  m_color = PIXEL_COLOR_X4(color);
  m_paint_code.enter_and_leave_outer_function();
}

void DiHorizontalLine::generate_instructions() {
  delete_instructions();
  if (m_flags & PRIM_FLAGS_CAN_DRAW) {
    EspFixups fixups;
    auto width = (uint16_t)m_width;
    DiLineSections sections;
    sections.add_piece(1, 0, 1, false);
    m_paint_code.draw_line(fixups, m_draw_x, m_draw_x, 0, m_width, &sections, m_flags, m_opaqueness, true);
    m_paint_code.do_fixups(fixups);
  }
}

void IRAM_ATTR DiHorizontalLine::paint(volatile uint32_t* p_scan_line, uint32_t line_index) {
  m_paint_code.call(this, p_scan_line, line_index);
}
