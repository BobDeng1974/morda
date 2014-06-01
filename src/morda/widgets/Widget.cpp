#include "Widget.hpp"

#include "../App.hpp"
#include "Container.hpp"
#include "../shaders/SimpleSingleColoringShader.hpp"
#include "../util/util.hpp"

using namespace morda;



Widget::Widget(const stob::Node& description){
	if(const stob::Node* p = description.Child("prop").node()){
		this->prop = p->Clone();
		this->prop->SetValue();//clear value of the prop node, we don't need it
	}

	if(const stob::Node* p = description.GetProperty("pos")){
		this->rect.p = morda::Vec2fFromSTOB(p);
	}else{
		this->rect.p.SetTo(0);
	}

	if(const stob::Node* p = description.GetProperty("dim")){
		this->rect.d = morda::Vec2fFromSTOB(p);
	}else{
		this->rect.d.SetTo(0);
	}

	if(const stob::Node* p = description.GetProperty("name")){
		this->name = p->Value();
	}

	if(const stob::Node* p = description.GetProperty("clip")){
		this->clip = p->AsBool();
	}
}



void Widget::RemoveFromParent(){
	if(ting::Ref<Container> p = this->parent){
		p->Remove(ting::Ref<Widget>(this));
	}
}



void Widget::SetRelayoutNeeded()throw(){
	this->minDimNeedsRecomputing = true;
	if(this->relayoutNeeded){
		return;
	}
	this->relayoutNeeded = true;
	if(ting::Ref<Container> p = this->parent){
		p->SetRelayoutNeeded();
	}
}



void Widget::RenderInternal(const morda::Matr4f& matrix)const{
	if(this->clip){
//		TRACE(<< "Widget::RenderInternal(): oldScissorBox = " << Rect2i(oldcissorBox[0], oldcissorBox[1], oldcissorBox[2], oldcissorBox[3]) << std::endl)

		//set scissor test
		Rect2i scissor(
				((matrix * Vec2f(0, 0) + Vec2f(1, 1)) / 2)
						.CompMulBy(App::Inst().viewportDim())
						.ConvertTo<int>(),
				this->rect.d.ConvertTo<int>()
			);

		Rect2i oldScissor;
		bool scissorTestWasEnabled = glIsEnabled(GL_SCISSOR_TEST);
		if(scissorTestWasEnabled){
			GLint osb[4];
			glGetIntegerv(GL_SCISSOR_BOX, osb);
			oldScissor = Rect2i(osb[0], osb[1], osb[2], osb[3]);
			scissor.Intersect(oldScissor);
		}else{
			glEnable(GL_SCISSOR_TEST);
		}
		
		glScissor(scissor.p.x, scissor.p.y, scissor.d.x, scissor.d.y);
		
		this->Render(matrix);
		
		if(scissorTestWasEnabled){
			glScissor(oldScissor.p.x, oldScissor.p.y, oldScissor.d.x, oldScissor.d.y);
		}else{
			glDisable(GL_SCISSOR_TEST);
		}
	}else{
		this->Render(matrix);
	}

	//render border
#ifdef M_MORDA_RENDER_WIDGET_BORDERS
	morda::SimpleSingleColoringShader& s = App::Inst().Shaders().simpleSingleColoring;
	s.Bind();
	morda::Matr4f matr(matrix);
	matr.Scale(this->Rect().d);
	s.SetMatrix(matr);

	if(this->IsHovered()){
		s.SetColor(morda::Vec3f(0, 1, 0));
	}else{
		s.SetColor(morda::Vec3f(1, 0, 1));
	}
	s.DrawQuad01(GL_LINE_LOOP);
#endif
}



template <bool is_down> void Widget::PassKeyEventToParent(key::Key keyCode){
	if(ting::Ref<Container> p = this->Parent()){
		p->HandleKeyEvent<is_down>(keyCode);
	}else{
		App& app = App::Inst();
		if(app.rootWidget && static_cast<Widget*>(app.rootWidget.operator->()) != this){
			app.rootWidget->HandleKeyEvent<is_down>(keyCode);
		}
	}
}



void Widget::PassKeyUpEventToParent(key::Key keyCode){
	this->PassKeyEventToParent<false>(keyCode);
}



void Widget::PassKeyDownEventToParent(key::Key keyCode){
	this->PassKeyEventToParent<true>(keyCode);
}



void Widget::Focus()throw(){
	ASSERT(App::Inst().ThisIsUIThread())

	if(this->IsFocused()){
		return;
	}

	if(ting::Ref<Widget> w = App::Inst().focusedWidget){
		w->isFocused = false;
	}
	App::Inst().focusedWidget = ting::WeakRef<Widget>(this);
	this->isFocused = true;
}



void Widget::Unfocus()throw(){
	ASSERT(App::Inst().ThisIsUIThread())

	if(!this->IsFocused()){
		return;
	}

	ASSERT(App::Inst().focusedWidget.GetRef() && App::Inst().focusedWidget.GetRef().operator->() == this)

	App::Inst().focusedWidget.Reset();
	this->isFocused = false;
}