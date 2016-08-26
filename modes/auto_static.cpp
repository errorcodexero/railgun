#include "auto_static.h"

unique_ptr<Mode> Auto_static::next_mode(Next_mode_info){
	return make_unique<Auto_static>();
}

Toplevel::Goal Auto_static::run(Run_info){
	return {};
}

#ifdef AUTO_STATIC_TEST
int main(){
	
}
#endif 
