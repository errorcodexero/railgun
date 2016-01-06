OBJDIR=tobj
H_FILES=*.h */*.h
CXXFLAGS=-Wall -Wextra -Werror -std=c++11 -g
export SHELL=/bin/bash
.PHONY: clean

all: tobj/log_control_monitor_test tobj/log_control_force_interface_test tobj/log_control_drivebase_test tobj/log_control_pump_test tobj/log_control_motor_check_test tobj/log_control_toplevel_mode_test tobj/log_control_main_test tobj/log_control_collector_test tobj/log_control_tote_sensors_test tobj/log_control_toplevel_test tobj/log_control_arm_test tobj/log_util_fixVictor_test tobj/log_util_posedge_trigger_test tobj/log_util_perf_tracker_test tobj/log_util_input_test tobj/log_util_jag_interface_test tobj/log_util_interface_test tobj/log_util_countup_timer_test tobj/log_util_output_test tobj/log_util_point_test tobj/log_util_driver_station_interface_test tobj/log_util_util_test tobj/log_util_settable_toggle_test tobj/log_util_negedge_trigger_test tobj/log_util_bound_tracker_test tobj/log_util_countdown_timer_test tobj/log_input_panel2015_test tobj/log_input_util_test 
$(OBJDIR)/%.o: %.cpp ${H_FILES}
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tobj/control_monitor_test: ${H_FILES} tobj/util/util.o control/monitor.cpp control/toplevel.o tobj/util/fixVictor.o tobj/util/interface.o tobj/util/jag_interface.o tobj/control/pump.o tobj/control/collector.o tobj/util/posedge_toggle.o tobj/control/force_interface.o tobj/control/drivebase.o tobj/util/driver_station_interface.o tobj/util/util.o tobj/util/countup_timer.o tobj/control/force.o tobj/util/posedge_trigger.o tobj/util/point.o tobj/util/negedge_trigger.o tobj/util/countdown_timer.o tobj/control/motor_check.o tobj/control/tote_sensors.o tobj/control/arm.o
	$(CXX) $(CXXFLAGS) -DMONITOR_TEST control/toplevel.o tobj/util/fixVictor.o tobj/util/interface.o tobj/util/jag_interface.o tobj/control/pump.o tobj/control/collector.o tobj/util/posedge_toggle.o tobj/control/force_interface.o tobj/control/drivebase.o tobj/util/driver_station_interface.o tobj/util/util.o tobj/util/countup_timer.o tobj/control/force.o tobj/util/posedge_trigger.o tobj/util/point.o tobj/util/negedge_trigger.o tobj/util/countdown_timer.o tobj/control/motor_check.o tobj/control/tote_sensors.o tobj/control/arm.o control/monitor.cpp -o tobj/control_monitor_test
tobj/log_control_monitor_test: tobj/control_monitor_test
	valgrind --error-exitcode=1 ./tobj/control_monitor_test &> tobj/log_control_monitor_test
tobj/control_force_interface_test: ${H_FILES} tobj/util/util.o control/force_interface.cpp util/posedge_trigger.o tobj/control/force.o tobj/util/interface.o tobj/util/util.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o
	$(CXX) $(CXXFLAGS) -DFORCE_INTERFACE_TEST util/posedge_trigger.o tobj/control/force.o tobj/util/interface.o tobj/util/util.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o control/force_interface.cpp -o tobj/control_force_interface_test
tobj/log_control_force_interface_test: tobj/control_force_interface_test
	valgrind --error-exitcode=1 ./tobj/control_force_interface_test &> tobj/log_control_force_interface_test
tobj/control_drivebase_test: ${H_FILES} tobj/util/util.o control/drivebase.cpp util/fixVictor.o tobj/util/interface.o tobj/util/jag_interface.o tobj/util/util.o tobj/util/driver_station_interface.o tobj/control/motor_check.o tobj/util/countdown_timer.o
	$(CXX) $(CXXFLAGS) -DDRIVEBASE_TEST util/fixVictor.o tobj/util/interface.o tobj/util/jag_interface.o tobj/util/util.o tobj/util/driver_station_interface.o tobj/control/motor_check.o tobj/util/countdown_timer.o control/drivebase.cpp -o tobj/control_drivebase_test
tobj/log_control_drivebase_test: tobj/control_drivebase_test
	valgrind --error-exitcode=1 ./tobj/control_drivebase_test &> tobj/log_control_drivebase_test
tobj/control_pump_test: ${H_FILES} tobj/util/util.o control/pump.cpp util/interface.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o
	$(CXX) $(CXXFLAGS) -DPUMP_TEST util/interface.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o control/pump.cpp -o tobj/control_pump_test
tobj/log_control_pump_test: tobj/control_pump_test
	valgrind --error-exitcode=1 ./tobj/control_pump_test &> tobj/log_control_pump_test
tobj/control_motor_check_test: ${H_FILES} tobj/util/util.o control/motor_check.cpp 
	$(CXX) $(CXXFLAGS) -DMOTOR_CHECK_TEST  control/motor_check.cpp -o tobj/control_motor_check_test
tobj/log_control_motor_check_test: tobj/control_motor_check_test
	valgrind --error-exitcode=1 ./tobj/control_motor_check_test &> tobj/log_control_motor_check_test
tobj/control_toplevel_mode_test: ${H_FILES} tobj/util/util.o control/toplevel_mode.cpp util/fixVictor.o tobj/util/jag_interface.o tobj/util/util.o tobj/control/toplevel.o tobj/control/drivebase.o tobj/util/countup_timer.o tobj/control/pump.o tobj/control/collector.o tobj/util/posedge_trigger.o tobj/util/negedge_trigger.o tobj/util/point.o tobj/util/countdown_timer.o tobj/control/motor_check.o tobj/control/tote_sensors.o tobj/util/interface.o tobj/util/driver_station_interface.o tobj/control/arm.o
	$(CXX) $(CXXFLAGS) -DTOPLEVEL_MODE_TEST util/fixVictor.o tobj/util/jag_interface.o tobj/util/util.o tobj/control/toplevel.o tobj/control/drivebase.o tobj/util/countup_timer.o tobj/control/pump.o tobj/control/collector.o tobj/util/posedge_trigger.o tobj/util/negedge_trigger.o tobj/util/point.o tobj/util/countdown_timer.o tobj/control/motor_check.o tobj/control/tote_sensors.o tobj/util/interface.o tobj/util/driver_station_interface.o tobj/control/arm.o control/toplevel_mode.cpp -o tobj/control_toplevel_mode_test
tobj/log_control_toplevel_mode_test: tobj/control_toplevel_mode_test
	valgrind --error-exitcode=1 ./tobj/control_toplevel_mode_test &> tobj/log_control_toplevel_mode_test
tobj/control_main_test: ${H_FILES} tobj/util/util.o control/main.cpp control/force.o tobj/control/force_interface.o tobj/util/fixVictor.o tobj/util/interface.o tobj/util/posedge_trigger.o tobj/util/posedge_toggle.o tobj/util/perf_tracker.o tobj/util/util.o tobj/util/countdown_timer.o tobj/util/bound_tracker.o tobj/control/toplevel.o tobj/util/point.o tobj/control/drivebase.o tobj/control/pump.o tobj/control/collector.o tobj/util/countup_timer.o tobj/util/negedge_trigger.o tobj/input/util.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/control/toplevel_mode.o tobj/control/monitor.o tobj/control/motor_check.o tobj/control/tote_sensors.o tobj/input/panel2015.o tobj/control/arm.o
	$(CXX) $(CXXFLAGS) -DMAIN_TEST control/force.o tobj/control/force_interface.o tobj/util/fixVictor.o tobj/util/interface.o tobj/util/posedge_trigger.o tobj/util/posedge_toggle.o tobj/util/perf_tracker.o tobj/util/util.o tobj/util/countdown_timer.o tobj/util/bound_tracker.o tobj/control/toplevel.o tobj/util/point.o tobj/control/drivebase.o tobj/control/pump.o tobj/control/collector.o tobj/util/countup_timer.o tobj/util/negedge_trigger.o tobj/input/util.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/control/toplevel_mode.o tobj/control/monitor.o tobj/control/motor_check.o tobj/control/tote_sensors.o tobj/input/panel2015.o tobj/control/arm.o control/main.cpp -o tobj/control_main_test
tobj/log_control_main_test: tobj/control_main_test
	valgrind --error-exitcode=1 ./tobj/control_main_test &> tobj/log_control_main_test
tobj/control_collector_test: ${H_FILES} tobj/util/util.o control/collector.cpp util/interface.o tobj/util/driver_station_interface.o tobj/util/util.o
	$(CXX) $(CXXFLAGS) -DCOLLECTOR_TEST util/interface.o tobj/util/driver_station_interface.o tobj/util/util.o control/collector.cpp -o tobj/control_collector_test
tobj/log_control_collector_test: tobj/control_collector_test
	valgrind --error-exitcode=1 ./tobj/control_collector_test &> tobj/log_control_collector_test
tobj/control_tote_sensors_test: ${H_FILES} tobj/util/util.o control/tote_sensors.cpp util/interface.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o
	$(CXX) $(CXXFLAGS) -DTOTE_SENSORS_TEST util/interface.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o control/tote_sensors.cpp -o tobj/control_tote_sensors_test
tobj/log_control_tote_sensors_test: tobj/control_tote_sensors_test
	valgrind --error-exitcode=1 ./tobj/control_tote_sensors_test &> tobj/log_control_tote_sensors_test
tobj/control_toplevel_test: ${H_FILES} tobj/util/util.o control/toplevel.cpp util/fixVictor.o tobj/util/posedge_trigger.o tobj/util/negedge_trigger.o tobj/util/countup_timer.o tobj/control/pump.o tobj/control/collector.o tobj/util/point.o tobj/control/drivebase.o tobj/util/interface.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o tobj/util/countdown_timer.o tobj/control/motor_check.o tobj/util/input.o tobj/util/output.o tobj/control/tote_sensors.o tobj/control/arm.o
	$(CXX) $(CXXFLAGS) -DTOPLEVEL_TEST util/fixVictor.o tobj/util/posedge_trigger.o tobj/util/negedge_trigger.o tobj/util/countup_timer.o tobj/control/pump.o tobj/control/collector.o tobj/util/point.o tobj/control/drivebase.o tobj/util/interface.o tobj/util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o tobj/util/countdown_timer.o tobj/control/motor_check.o tobj/util/input.o tobj/util/output.o tobj/control/tote_sensors.o tobj/control/arm.o control/toplevel.cpp -o tobj/control_toplevel_test
tobj/log_control_toplevel_test: tobj/control_toplevel_test
	valgrind --error-exitcode=1 ./tobj/control_toplevel_test &> tobj/log_control_toplevel_test
tobj/control_arm_test: ${H_FILES} tobj/util/util.o control/arm.cpp util/interface.o tobj/util/driver_station_interface.o tobj/util/util.o
	$(CXX) $(CXXFLAGS) -DARM_TEST util/interface.o tobj/util/driver_station_interface.o tobj/util/util.o control/arm.cpp -o tobj/control_arm_test
tobj/log_control_arm_test: tobj/control_arm_test
	valgrind --error-exitcode=1 ./tobj/control_arm_test &> tobj/log_control_arm_test
tobj/util_fixVictor_test: ${H_FILES} tobj/util/util.o util/fixVictor.cpp 
	$(CXX) $(CXXFLAGS) -DFIXVICTOR_TEST  util/fixVictor.cpp -o tobj/util_fixVictor_test
tobj/log_util_fixVictor_test: tobj/util_fixVictor_test
	valgrind --error-exitcode=1 ./tobj/util_fixVictor_test &> tobj/log_util_fixVictor_test
tobj/util_posedge_trigger_test: ${H_FILES} tobj/util/util.o util/posedge_trigger.cpp 
	$(CXX) $(CXXFLAGS) -DPOSEDGE_TRIGGER_TEST  util/posedge_trigger.cpp -o tobj/util_posedge_trigger_test
tobj/log_util_posedge_trigger_test: tobj/util_posedge_trigger_test
	valgrind --error-exitcode=1 ./tobj/util_posedge_trigger_test &> tobj/log_util_posedge_trigger_test
tobj/util_perf_tracker_test: ${H_FILES} tobj/util/util.o util/perf_tracker.cpp 
	$(CXX) $(CXXFLAGS) -DPERF_TRACKER_TEST  util/perf_tracker.cpp -o tobj/util_perf_tracker_test
tobj/log_util_perf_tracker_test: tobj/util_perf_tracker_test
	valgrind --error-exitcode=1 ./tobj/util_perf_tracker_test &> tobj/log_util_perf_tracker_test
tobj/util_input_test: ${H_FILES} tobj/util/util.o util/input.cpp 
	$(CXX) $(CXXFLAGS) -DINPUT_TEST  util/input.cpp -o tobj/util_input_test
tobj/log_util_input_test: tobj/util_input_test
	valgrind --error-exitcode=1 ./tobj/util_input_test &> tobj/log_util_input_test
tobj/util_jag_interface_test: ${H_FILES} tobj/util/util.o util/jag_interface.cpp util/util.o
	$(CXX) $(CXXFLAGS) -DJAG_INTERFACE_TEST util/util.o util/jag_interface.cpp -o tobj/util_jag_interface_test
tobj/log_util_jag_interface_test: tobj/util_jag_interface_test
	valgrind --error-exitcode=1 ./tobj/util_jag_interface_test &> tobj/log_util_jag_interface_test
tobj/util_interface_test: ${H_FILES} tobj/util/util.o util/interface.cpp util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o
	$(CXX) $(CXXFLAGS) -DINTERFACE_TEST util/jag_interface.o tobj/util/driver_station_interface.o tobj/util/util.o util/interface.cpp -o tobj/util_interface_test
tobj/log_util_interface_test: tobj/util_interface_test
	valgrind --error-exitcode=1 ./tobj/util_interface_test &> tobj/log_util_interface_test
tobj/util_countup_timer_test: ${H_FILES} tobj/util/util.o util/countup_timer.cpp 
	$(CXX) $(CXXFLAGS) -DCOUNTUP_TIMER_TEST  util/countup_timer.cpp -o tobj/util_countup_timer_test
tobj/log_util_countup_timer_test: tobj/util_countup_timer_test
	valgrind --error-exitcode=1 ./tobj/util_countup_timer_test &> tobj/log_util_countup_timer_test
tobj/util_output_test: ${H_FILES} tobj/util/util.o util/output.cpp 
	$(CXX) $(CXXFLAGS) -DOUTPUT_TEST  util/output.cpp -o tobj/util_output_test
tobj/log_util_output_test: tobj/util_output_test
	valgrind --error-exitcode=1 ./tobj/util_output_test &> tobj/log_util_output_test
tobj/util_point_test: ${H_FILES} tobj/util/util.o util/point.cpp 
	$(CXX) $(CXXFLAGS) -DPOINT_TEST  util/point.cpp -o tobj/util_point_test
tobj/log_util_point_test: tobj/util_point_test
	valgrind --error-exitcode=1 ./tobj/util_point_test &> tobj/log_util_point_test
tobj/util_driver_station_interface_test: ${H_FILES} tobj/util/util.o util/driver_station_interface.cpp util/util.o
	$(CXX) $(CXXFLAGS) -DDRIVER_STATION_INTERFACE_TEST util/util.o util/driver_station_interface.cpp -o tobj/util_driver_station_interface_test
tobj/log_util_driver_station_interface_test: tobj/util_driver_station_interface_test
	valgrind --error-exitcode=1 ./tobj/util_driver_station_interface_test &> tobj/log_util_driver_station_interface_test
tobj/util_util_test: ${H_FILES} tobj/util/util.o util/util.cpp util/point.o
	$(CXX) $(CXXFLAGS) -DUTIL_TEST util/point.o util/util.cpp -o tobj/util_util_test
tobj/log_util_util_test: tobj/util_util_test
	valgrind --error-exitcode=1 ./tobj/util_util_test &> tobj/log_util_util_test
tobj/util_settable_toggle_test: ${H_FILES} tobj/util/util.o util/settable_toggle.cpp util/posedge_trigger.o
	$(CXX) $(CXXFLAGS) -DSETTABLE_TOGGLE_TEST util/posedge_trigger.o util/settable_toggle.cpp -o tobj/util_settable_toggle_test
tobj/log_util_settable_toggle_test: tobj/util_settable_toggle_test
	valgrind --error-exitcode=1 ./tobj/util_settable_toggle_test &> tobj/log_util_settable_toggle_test
tobj/util_negedge_trigger_test: ${H_FILES} tobj/util/util.o util/negedge_trigger.cpp 
	$(CXX) $(CXXFLAGS) -DNEGEDGE_TRIGGER_TEST  util/negedge_trigger.cpp -o tobj/util_negedge_trigger_test
tobj/log_util_negedge_trigger_test: tobj/util_negedge_trigger_test
	valgrind --error-exitcode=1 ./tobj/util_negedge_trigger_test &> tobj/log_util_negedge_trigger_test
tobj/util_bound_tracker_test: ${H_FILES} tobj/util/util.o util/bound_tracker.cpp 
	$(CXX) $(CXXFLAGS) -DBOUND_TRACKER_TEST  util/bound_tracker.cpp -o tobj/util_bound_tracker_test
tobj/log_util_bound_tracker_test: tobj/util_bound_tracker_test
	valgrind --error-exitcode=1 ./tobj/util_bound_tracker_test &> tobj/log_util_bound_tracker_test
tobj/util_countdown_timer_test: ${H_FILES} tobj/util/util.o util/countdown_timer.cpp 
	$(CXX) $(CXXFLAGS) -DCOUNTDOWN_TIMER_TEST  util/countdown_timer.cpp -o tobj/util_countdown_timer_test
tobj/log_util_countdown_timer_test: tobj/util_countdown_timer_test
	valgrind --error-exitcode=1 ./tobj/util_countdown_timer_test &> tobj/log_util_countdown_timer_test
tobj/input_panel2015_test: ${H_FILES} tobj/util/util.o input/panel2015.cpp util/util.o tobj/input/util.o tobj/util/driver_station_interface.o tobj/util/jag_interface.o tobj/util/interface.o
	$(CXX) $(CXXFLAGS) -DPANEL2015_TEST util/util.o tobj/input/util.o tobj/util/driver_station_interface.o tobj/util/jag_interface.o tobj/util/interface.o input/panel2015.cpp -o tobj/input_panel2015_test
tobj/log_input_panel2015_test: tobj/input_panel2015_test
	valgrind --error-exitcode=1 ./tobj/input_panel2015_test &> tobj/log_input_panel2015_test
tobj/input_util_test: ${H_FILES} tobj/util/util.o input/util.cpp 
	$(CXX) $(CXXFLAGS) -DINPUT_UTIL_TEST  input/util.cpp -o tobj/input_util_test
tobj/log_input_util_test: tobj/input_util_test
	valgrind --error-exitcode=1 ./tobj/input_util_test &> tobj/log_input_util_test
clean:
	rm -f *_test
	find -name \*.o | xargs rm -f
