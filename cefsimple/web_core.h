#include <include/cef_client.h>
#include <string>

class RenderHandler;
class BrowserClient;

class WebCore {
public:
	enum {

	};
public:
	WebCore(const std::string &url);
	~WebCore();

	void reshape(int w, int h);

	void mouseMove(int x, int y);
	void mouseClick(CefBrowserHost::MouseButtonType btn, bool mouse_up);
	void keyPress(int key, bool pressed);

	RenderHandler* render_handler() const { return render_handler_; }

private:
	int mouse_x_;
	int mouse_y_;

	CefRefPtr<CefBrowser> browser_;
	CefRefPtr<BrowserClient> client_;

	RenderHandler* render_handler_;
};
