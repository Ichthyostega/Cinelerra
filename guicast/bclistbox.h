#ifndef BCLISTBOX_H
#define BCLISTBOX_H

#include "bcdragwindow.inc"
#include "bclistboxitem.inc"
#include "bcpixmap.inc"
#include "bcpopuplistbox.inc"
#include "bcscrollbar.h"
#include "bcsubwindow.h"
#include "bctoggle.h"
#include "colors.h"

#define BCPOPUPLISTBOX_W 25
#define BCPOPUPLISTBOX_H 25

#define LISTBOX_UP 0
#define LISTBOX_HIGH 1
#define LISTBOX_DN 2

#define MIN_COLUMN_WIDTH 10

enum
{
	BCLISTBOX_NO_OPERATION,
	BCLISTBOX_DRAG_DIVISION,   // Dragging column division
	BCLISTBOX_SELECT,          // Select item
	BCLISTBOX_SELECT_RECT,     // Selection rectangle
	BCLISTBOX_WHEEL            // Wheel mouse
};


class BC_ListBoxYScroll : public BC_ScrollBar
{
public:
	BC_ListBoxYScroll(BC_ListBox *listbox, 
	                  int total_height, 
					  int view_height, 
	                  int position);
	~BC_ListBoxYScroll();
	int handle_event();
private:
	BC_ListBox *listbox;
};

class BC_ListBoxXScroll : public BC_ScrollBar
{
public:
	BC_ListBoxXScroll(BC_ListBox *listbox, 
	                  int total_width, 
					  int view_width,
	                  int position);
	~BC_ListBoxXScroll();
	int handle_event();
private:
	BC_ListBox *listbox;
};

class BC_ListBoxToggle : public BC_Toggle
{
public:
	BC_ListBoxToggle(BC_ListBox *listbox, BC_ListBoxItem *item, int x, int y);
	int handle_event();
	void update(BC_ListBoxItem *item, int x, int y);
	BC_ListBox *listbox;
	BC_ListBoxItem *item;
};


class BC_ListBox : public BC_SubWindow
{
public:
	BC_ListBox(int x, 
		int y, 
		int w, 
		int h,
		int display_format,                   // Display text list or icons
		ArrayList<BC_ListBoxItem*> *data = 0, // Each column has an ArrayList of BC_ListBoxItems.
		char **column_titles = 0,             // Titles for columns.  Set to 0 for no titles
		int *column_width = 0,                // width of each column
		int columns = 1,                      // Total columns.  Only 1 in icon mode
		int yposition = 0,                    // Pixel of top of window.
		int popup = 0,                        // If this listbox is a popup window
		int selection_mode = LISTBOX_SINGLE,  // Select one item or multiple items
		int icon_position = ICON_LEFT,        // Position of icon relative to text of each item
		int allow_drag = 0);                  // Allow user to drag icons around
	virtual ~BC_ListBox();

	friend class BC_PopupListBox;

	int initialize();

// User event handler for new selections
	virtual int selection_changed() { return 0; };
// User event handler for triggering a selection
	virtual int handle_event() { return 0; };
// User event handler for a column resize
	virtual int column_resize_event() { return 0; };
// Draw background on bg_surface
	virtual void draw_background();


// Get the item in the given column which is the selection_number of the total
// selected rows.  Returns 0 on failure.
	BC_ListBoxItem* get_selection(int column, int selection_number);
	BC_ListBoxItem* get_selection_recursive(ArrayList<BC_ListBoxItem*> *data,
		int column,
		int selection_number);

// Get the flat index of the item in the given column which is the selection_number
// of the total selected rows.  Returns -1 on failure.  The column
// argument is really useless because it only checks column 1 and returns the row
// number.
	int get_selection_number(int column, int selection_number);
	int get_selection_number_recursive(ArrayList<BC_ListBoxItem*> *data,
		int column,
		int selection_number,
		int *counter = 0);

	virtual int evaluate_query(int list_item, char *string);
	void expand_item(BC_ListBoxItem *item, int expand);
// Collapse all items
	static void collapse_recursive(ArrayList<BC_ListBoxItem*> *data);
// Convert recursive pointer to flat index.
// The pointer can be any item in the row corresponding to the index.
// Returns -1 if no item was found.
	int item_to_index(ArrayList<BC_ListBoxItem*> *data,
		BC_ListBoxItem *item,
		int *counter = 0);
// Convert flat index and column to desired item.
	BC_ListBoxItem* index_to_item(ArrayList<BC_ListBoxItem*> *data,
		int number,
		int column,
		int *counter = 0);
// Get all items with recursion for text mode
	static int get_total_items(ArrayList<BC_ListBoxItem*> *data, int *result = 0);


	virtual int button_press_event();
	int button_release_event();
	int cursor_enter_event();
	int cursor_leave_event();
	int cursor_motion_event();
	virtual int drag_start_event();
	virtual int drag_motion_event();
	virtual int drag_stop_event();
	int deactivate();
// After popping up a menu call this to interrupt the selection process
	void deactivate_selection();
	int activate();
	virtual int keypress_event();
	int translation_event();
	int repeat_event(long repeat_id);
	BC_DragWindow* get_drag_popup();

// change the contents
	int update(ArrayList<BC_ListBoxItem*> *data,
						char **column_titles,
						int *column_widths,
						int columns,
						int xposition = 0,
						int yposition = 0, 
						int highlighted_number = -1,  // Flat index of item cursor is over
						int recalc_positions = 0,     // set all autoplace flags to 1
						int draw = 1);
	void center_selection();
	void update_format(int display_format, int redraw);
	int get_format();

// Allow scrolling when dragging items
	void set_drag_scroll(int value);
	int set_selection_mode(int mode);
	int set_yposition(int position, int draw_items = 1);
	int get_yposition();
	int set_xposition(int position);
	int get_xposition();
// Return the flat index of the highlighted item
	int get_highlighted_item();
	int get_yscroll_x();
	int get_yscroll_y();
	int get_yscroll_height();
	int get_xscroll_x();
	int get_xscroll_y();
	int get_xscroll_width();
	int get_column_offset(int column);
	int get_column_width(int column, int clamp_right = 0);
	int get_w();
	int get_h();
	int get_display_mode();
	void reset_query();
	int reposition_window(int x, int y, int w = -1, int h = -1);
	BC_Pixmap* get_bg_surface();
// Set all items for autoplacement with recursion into sublists
	void set_autoplacement(ArrayList<BC_ListBoxItem*> *data,
		int do_icon, 
		int do_text);
// Set selection status on all items with recursion
	void set_all_selected(ArrayList<BC_ListBoxItem*> *data, int value);
// Set selection status of single row with recursion
// item_number - the flat index of the row to select
	void set_selected(ArrayList<BC_ListBoxItem*> *data, 
		int item_number, 
		int value,
		int *counter = 0);

// Called by keypress_event for cursor up.  Selects previous and next visible 
// row, skipping desired number of visible rows.
	int select_previous(int skip, 
		BC_ListBoxItem *selected_item = 0,
		int *counter = 0,
		ArrayList<BC_ListBoxItem*> *data = 0,
		int *got_it = 0,
		int *got_second = 0);
	int select_next(int skip, 
		BC_ListBoxItem *selected_item = 0,
		int *counter = 0,
		ArrayList<BC_ListBoxItem*> *data = 0,
		int *got_it = 0,
		int *got_second = 0);

// Called by cursor_motion_event to select different item if selection_number
// changed.  Returns 1 if redraw is required.
	int update_selection(ArrayList<BC_ListBoxItem*> *data, 
		int selection_number,
		int *counter = 0);

	static void dump(ArrayList<BC_ListBoxItem*> *data, 
		int columns, 
		int indent = 0);


private:
	void delete_columns();
	void set_columns(char **column_titles, 
		int *column_widths, 
		int columns);
// Draw the button for a popup listbox
	int draw_face();
	int draw_items();
	int draw_border();
	int draw_rectangle();
	void draw_text_recursive(ArrayList<BC_ListBoxItem*> *data, 
		int column,
		int indent,
		int *current_toggle);
// Returns 1 if selection changed
	int query_list();
	void init_column_width();
	void reset_cursor(int new_cursor);
// Fix boundary conditions after resize
	void column_width_boundaries();
// Recursive function to get the first item selected in text mode.
// Returns > -1 only if it got it.  Always increments *result
	int get_first_selection(ArrayList<BC_ListBoxItem*> *data, int *result = 0);
// Recursive function to get the last item selected in text mode.
// Returns > -1 only if it got it.  Always increments *result
	int get_last_selection(ArrayList<BC_ListBoxItem*> *data, int *result = 0);
// Called by button_press_event and cursor_motion_event to expand the selection.
// Returns 1 if redraw is required.
	int expand_selection(int button_press, int selection_number);
// Called by button_press_event and cursor_motion_event 
// to select a range in text mode
	void select_range(ArrayList<BC_ListBoxItem*> *data,
		int start,
		int end,
		int *current = 0);
// Called by button_press_event to toggle the selection status of a single item.
// Called for both text and icon mode.  In text mode it's recursive and fills
// the entire row with the first item's value.  Returns 1 when the item was toggled.
	int toggle_item_selection(ArrayList<BC_ListBoxItem*> *data,
		int selection_number,
		int *counter = 0);
// Set value of selected in all selected items to new value
	void promote_selections(ArrayList<BC_ListBoxItem*> *data,
		int old_value,
		int new_value);


	int test_divisions(int cursor_x, int cursor_y, int &new_cursor);

	int get_title_h();
	int calculate_item_coords();
	void BC_ListBox::calculate_last_coords_recursive(
		ArrayList<BC_ListBoxItem*> *data,
		int *icon_x,
		int *next_icon_x,
		int *next_icon_y,
		int *next_text_y,
		int top_level);
	void calculate_item_coords_recursive(
		ArrayList<BC_ListBoxItem*> *data,
		int *icon_x,
		int *next_icon_x,
		int *next_icon_y,
		int *next_text_y,
		int top_level);

	int get_items_width();
	int get_items_height(ArrayList<BC_ListBoxItem*> *data, int columns, int top_level);
	int get_icon_w(BC_ListBoxItem *item);
	int get_icon_h(BC_ListBoxItem *item);
	int get_item_x(BC_ListBoxItem *item);
	int get_item_y(BC_ListBoxItem *item);
	int get_item_w(BC_ListBoxItem *item);
	int get_item_h(BC_ListBoxItem *item);
	int get_item_highlight(ArrayList<BC_ListBoxItem*> *data, int column, int item);
	int get_item_color(ArrayList<BC_ListBoxItem*> *data, int column, int item);
	int get_icon_mask(BC_ListBoxItem *item, int &x, int &y, int &w, int &h);
	int get_text_mask(BC_ListBoxItem *item, int &x, int &y, int &w, int &h);
// Copy sections of the bg_surface to the gui
	void clear_listbox(int x, int y, int w, int h);
	void test_drag_scroll(int &redraw, int cursor_x, int cursor_y);
	void move_vertical(int pixels);
	void move_horizontal(int pixels);
	void fix_positions();

	int get_scrollbars();
	void update_scrollbars();

// Flat index of the item the cursor is over.  
// Points *item_return to the first item in the row
// if it's nonzero.  Returns -1 if no item was found.
// expanded - 1 if items in this table should be tested for cursor coverage
	int cursor_item(ArrayList<BC_ListBoxItem*> *data,
		int cursor_x, 
		int cursor_y, 
		BC_ListBoxItem **item_return = 0,
		int *counter = 0,
		int expanded = 1);
// Select the items in the rectangle and deselect the items outside of it.
// Returns 1 if redraw is required.
	int select_rectangle(ArrayList<BC_ListBoxItem*> *data,
		int x1, 
		int y1,
		int x2, 
		int y2);
// Convert the row of the item to a pointer.
	BC_ListBoxItem* number_to_item(int row);
	int reposition_item(ArrayList<BC_ListBoxItem*> *data,
		int selection_number,
		int x,
		int y,
		int *counter = 0);
// Move selected items to src_items
	void move_selection(ArrayList<BC_ListBoxItem*> *dst,
		ArrayList<BC_ListBoxItem*> *src);
// Put items from the src table into the data table starting at flat item number
// destination.
	int put_selection(ArrayList<BC_ListBoxItem*> *data,
		ArrayList<BC_ListBoxItem*> *src,
		int destination,
		int *counter = 0);

	int center_selection(int selection,
		ArrayList<BC_ListBoxItem*> *data = 0,
		int *counter = 0);

// Array of one list of pointers for each column
	ArrayList<BC_ListBoxItem*> *data;
// Mode
	int popup;
// Dimensions for a popup if there is one
	int popup_w, popup_h;
// pixel of top of display relative to top of list
	int yposition;
// pixel of left display relative to first column
	int xposition;
// dimensions of a row in the list
	int row_height, row_ascent, row_descent;
// item cursor is over.  May not exist in tables.
// Must be an index since this is needed to change the database.
	int highlighted_item;
	BC_ListBoxItem* highlighted_ptr;
// Item being dragged or last item selected in a double click operation
	int selection_number;
// Used in button_press_event and button_release_event to detect double clicks
	int selection_number1, selection_number2;
// When cursor is over the listbox or button.
	int highlighted;
	int selection_mode;
	int display_format;
	int icon_position;
// Scrollbars are created as needed
	BC_ListBoxXScroll *xscrollbar;
	BC_ListBoxYScroll *yscrollbar;
	ArrayList<BC_ListBoxToggle*> expanders;
	char query[BCTEXTLEN];
// Window containing the listbox
	BC_WindowBase *gui;
// Size of the popup if there is one
	char **column_titles;
	int *column_width;
	int default_column_width[1];
	int columns;
	int items_per_column;
	int view_h, view_w;
	int title_h;
	int active;
// In BCLISTBOX_SELECT mode determines the value to set items to
	int new_value;
	int need_xscroll, need_yscroll;
	int allow_drag;
	int allow_drag_scroll;
// Selection range being extended
	int selection_start, selection_end, selection_center;
// Background color of listbox
	int list_background;
	BC_Pixmap *bg_tile;
// Drag icon for text mode
	BC_Pixmap *drag_icon;
// Popup button
	BC_Pixmap *images[3];
// Background for drawing on
	BC_Pixmap *bg_surface;
// Default background picon
	BC_Pixmap *bg_pixmap;
	int status;
	int button_releases;
	int current_cursor;
// Starting coordinates of rectangle
	int rect_x1, rect_y1;
	int rect_x2, rect_y2;
// Gradually being expanded into the state of the list box.
	int current_operation;


// Division the cursor is operating on when resizing
	int highlighted_division;
// Window for dragging
	BC_DragWindow *drag_popup;
};




#endif
