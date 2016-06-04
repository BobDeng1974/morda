#include "CheckBox.hpp"
#include "../App.hpp"


using namespace morda;



namespace{

const char* D_Layout = R"qwertyuiop(
		ImageLabel{
			name{morda_checkbox_check}
			image{morda_img_checkbox}
			layout{
				fillX{true} fillY{true}
			}
		}
	)qwertyuiop";

}



CheckBox::CheckBox(const stob::Node* chain) :
		Widget(chain),
		ToggleButton(chain),
		NinePatch(stob::parse(D_Layout).get())
{
	this->checkWidget = this->content().findChildByName("morda_checkbox_check");
	this->checkWidget->setVisible(this->isChecked());
	
	this->setNinePatch(morda::App::inst().resMan.load<ResNinePatch>("morda_npt_checkbox_bg"));
}



void CheckBox::onCheckedChanged(){
	this->checkWidget->setVisible(this->isChecked());
	this->clearCache();
	this->ToggleButton::onCheckedChanged();
}
