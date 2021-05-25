#include"dealdata.hpp"

using namespace boost::asio;


std::string dealdata()   
{      
	//std::string serbuf;
	char serialbuf[500];
	io_service iosev;
	serial_port sp1(iosev, "COM5");
	sp1.set_option(serial_port::baud_rate(115200));
	sp1.set_option(serial_port::flow_control(serial_port::flow_control::none));
	sp1.set_option(serial_port::parity(serial_port::parity::none));
	sp1.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
	sp1.set_option(serial_port::character_size(8));

	

	/*deadline_timer timer(iosev);
	timer.expires_from_now(boost::posix_time::millisec(800));
	timer.async_wait(boost::bind(&serial_port::cancel, boost::ref(sp1)));*/

	//������            Ŀǰȷ���Ƕ�����׼ȷ�ģ��������������
	
    int count1 = sp1.read_some(buffer(serialbuf));         //������read��sp1��buffer������û�з���ֵ��server崻�������������
	std::cout << count1 << std::endl;
    std::cout << serialbuf <<"  -- server read from serial port" << std::endl;
	std::cout << std::endl;

   //system("pause");          

	return  std::string(serialbuf);
	
}