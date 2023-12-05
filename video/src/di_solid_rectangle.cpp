// di_solid_rectangle.cpp - Function definitions for drawing solid rectangles
//
// A solid rectangle is filled with a single color.
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

#include "di_solid_rectangle.h"

DiSolidRectangle::DiSolidRectangle() {
}

void DiSolidRectangle::make_rectangle(uint16_t flags, int32_t x, int32_t y, uint32_t width, uint32_t height, uint8_t color) {
  m_flags = flags;
  m_opaqueness = DiPrimitive::normal_alpha_to_opaqueness(color);
  m_rel_x = x;
  m_rel_y = y;
  m_width = width;
  m_height = height;
  m_color = PIXEL_COLOR_X4(color);
  m_paint_code.enter_and_leave_outer_function();
}

void DiSolidRectangle::generate_instructions() {
  delete_instructions();
  if (m_flags & PRIM_FLAGS_CAN_DRAW) {
    EspFixups fixups;
    generate_code_for_positions(fixups, m_width, m_height);
    m_paint_code.do_fixups(fixups);
    set_current_paint_pointer(m_width, m_height);
  }
}

void DiSolidRectangle::generate_code_for_left_edge(EspFixups& fixups, uint32_t y_line, uint32_t width, uint32_t height, uint32_t hidden, uint32_t visible) {
  start_paint_section();
  auto draw_width = (m_draw_x_extent - m_draw_x) - hidden;
  auto draw_height = m_draw_y_extent - m_draw_y;
  DiLineSections sections;
  sections.add_piece(1, 0, (uint16_t)draw_width, false);
  m_paint_code.draw_line_as_outer_fcn(fixups, m_draw_x, 0, &sections, m_flags, m_opaqueness);
}

void DiSolidRectangle::generate_code_for_right_edge(EspFixups& fixups, uint32_t y_line, uint32_t width, uint32_t height, uint32_t hidden, uint32_t visible) {
  start_paint_section();
  auto draw_width = (m_draw_x_extent - m_draw_x) - hidden;
  auto draw_height = m_draw_y_extent - m_draw_y;
  DiLineSections sections;
  sections.add_piece(1, 0, (uint16_t)draw_width, false);
  m_paint_code.draw_line_as_outer_fcn(fixups, m_draw_x, m_draw_x, &sections, m_flags, m_opaqueness);
}

void DiSolidRectangle::generate_code_for_draw_area(EspFixups& fixups, uint32_t y_line, uint32_t width, uint32_t height, uint32_t hidden, uint32_t visible) {
  start_paint_section();
  auto draw_width = m_draw_x_extent - m_draw_x;
  auto draw_height = m_draw_y_extent - m_draw_y;
  DiLineSections sections;
  sections.add_piece(1, 0, (uint16_t)draw_width, false);
  m_paint_code.draw_line_as_outer_fcn(fixups, m_draw_x, m_draw_x, &sections, m_flags, m_opaqueness);
}

void IRAM_ATTR DiSolidRectangle::paint(volatile uint32_t* p_scan_line, uint32_t line_index) {
  (*(m_cur_paint_ptr.m_simple))(this, p_scan_line, line_index);
}