#ifndef x_chan_h
#define x_chan_h

#include"x_khop.h"

class x_chan
{
  private:
    x_khop *_khop_dau_goi;
    x_khop *_khop_dui;
  public:
    x_chan(x_khop &_khop_dui, x_khop &_khop_dau_goi);
    bool moveTo(double vitri_khop_dui, double vitri_khop_dau_goi);
    void runToPosition();
    void echo_position(); // echo khop dui roi den khop dau goi
    void home(bool khop_dui, bool khop_dau_goi);
    void run();
    void setMaxSpeed(float khop_dui, float khop_dau_goi);
    void setAcceleration(float khop_dui, float khop_dau_goi);
    bool is_running();
    ~x_chan();

};

x_chan::x_chan(x_khop &khop_dui, x_khop &khop_dau_goi)
{
  _khop_dau_goi = &khop_dau_goi;
  _khop_dui = &khop_dui;
}
x_chan::~x_chan()
{
	delete[] _khop_dau_goi;
	delete[] _khop_dui;
}

bool x_chan::moveTo(double vitri_khop_dui, double vitri_khop_dau_goi)
{
  _khop_dau_goi->moveTo(vitri_khop_dau_goi);
  _khop_dui->moveTo(vitri_khop_dui);
}

void x_chan::runToPosition()
{
  for (;;)
  {
    if (_khop_dau_goi->is_running() == false && _khop_dui->is_running() == false)
    {
      break;
    }

    this->run();
  }
}
void x_chan::run()
{
  _khop_dau_goi->run();
  _khop_dui->run();
}

void x_chan::home(bool khop_dui, bool khop_dau_goi)
{
  if (khop_dui == true && khop_dau_goi == false)
  {
    _khop_dui->home();
  }
  if (khop_dui == false && khop_dau_goi == true)
  {
    _khop_dau_goi->home();
  }
  if (khop_dui == true && khop_dau_goi == true)
  {
    _khop_dau_goi->home();
    _khop_dui->home();
  }

}

void x_chan::setMaxSpeed(float khop_dui_speed, float khop_dau_goi_speed)
{
  _khop_dui->setMaxSpeed(khop_dui_speed);
  _khop_dau_goi->setMaxSpeed(khop_dau_goi_speed);
}

void x_chan::setAcceleration(float khop_dui_accel, float khop_dau_goi_accel)
{
	_khop_dui->setAcceleration(khop_dui_accel);
	_khop_dau_goi->setAcceleration(khop_dau_goi_accel);
}

bool x_chan::is_running()
{
	bool ret;
	if (_khop_dui->is_running() == false && _khop_dau_goi->is_running() == false){
		ret = false;
	}
	else{
		ret = true;
	}
	return ret;
}

#endif
