DevTest/api/DEVICE/DevTest: "dev/test/10",\
                            "dev/test/11",\
                            "dev/test/12"

#############################################
# CLASS DevTest

CLASS/DevTest->AllowedAccessCmd: IOFloat
CLASS/DevTest->property1: "property test 1"
CLASS/DevTest->property2: "property test 2",\
                          "line 2",\
                          "lien 3"

# CLASS DevTest attribute properties

CLASS/DevTest/String_spec_attr->format: "Class format"
CLASS/DevTest/String_spec_attr->label: "Class label"
CLASS/DevTest/String_spec_attr->unit: "Class unit"
CLASS/DevTest/Added_short_attr->label: "From db (class)"


# DEVICE dev/test/10 properties 

dev/test/10->cmd_min_poll_period: IOExcept,\
                                 500
dev/test/10->min_poll_period: 200
dev/test/10->polled_attr: state,\
                         3000
dev/test/10->poll_old_factor: 4
dev/test/10->tst_property: 25
dev/test/10->__SubDevices: dev/test/11,\
                          dev/test/12

# DEVICE dev/test/10 attribute properties

dev/test/10/Short_attr->archive_period: 15000
dev/test/10/Short_attr->archive_rel_change: 10
dev/test/10/Short_attr->event_period: 600
dev/test/10/Short_attr->format: 14878
dev/test/10/Long_attr->format: %c
dev/test/10/Long_attr->max_alarm: 1500
dev/test/10/Long_attr->min_alarm: 1000
dev/test/10/Double_attr->abs_change: 33333
dev/test/10/Double_attr->event_period: 1000
dev/test/10/Double_attr->rel_change: 99.99
dev/test/10/String_attr->format: %s
dev/test/10/String_spec_attr->event_period: 1000
dev/test/10/String_spec_attr->label: "Class label"
dev/test/10/String_spec_attr->unit: "Class unit"
dev/test/10/Short_attr_rw->display_unit: 14938
dev/test/10/Short_attr_rw->event_period: 1000
dev/test/10/Short_attr_rw->format: 14938
dev/test/10/Short_attr_w->event_period: 1000
dev/test/10/Short_attr_w->__value: 10
dev/test/10/Long_attr_w->description: "Test description"
dev/test/10/Long_attr_w->display_unit: "Et ta soeur"
dev/test/10/Long_attr_w->event_period: 1000
dev/test/10/Long_attr_w->label: "Test label"
dev/test/10/Long_attr_w->max_alarm: 99999
dev/test/10/Long_attr_w->max_value: 100000
dev/test/10/Long_attr_w->min_alarm: 1
dev/test/10/Long_attr_w->min_value: 0
dev/test/10/Long_attr_w->standard_unit: 100
dev/test/10/Long_attr_w->unit: Kilogramme
dev/test/10/Double_attr_w->max_value: 70
dev/test/10/Double_attr_w->min_value: 0
dev/test/10/String_attr_w->event_period: 1000
dev/test/10/String_attr_w->format: %c
dev/test/10/String_attr_w->__value: "Hello world"
dev/test/10/Event_change_tst->abs_change: 1
dev/test/10/Event_change_tst->archive_abs_change: 1
dev/test/10/Event_change_tst->event_period: 1000
dev/test/10/Event64_change_tst->abs_change: 1
dev/test/10/Event64_change_tst->event_period: 1000
dev/test/10/Boolean_attr_w->__value: false
dev/test/10/Float_spec_attr_rw->1: 15.5
dev/test/10/Float_spec_attr_rw->3.5: max_value
dev/test/10/slow_actuator->abs_change: 1
dev/test/10/fast_actuator->abs_change: 1
dev/test/10/Long64_attr_rw->event_period: 1000
dev/test/10/SlowAttr->event_period: 1000
dev/test/10/Encoded_attr->abs_change: 1
dev/test/10/Encoded_attr->event_period: 1000
dev/test/10/Encoded_image->event_period: 1000
dev/test/10/Encoded_image->format: RawImage
# DEVICE dev/test/11 properties 

dev/test/11->__SubDevices: dev/test/11

# DEVICE dev/test/11 attribute properties

dev/test/11/Double_attr->abs_change: 1
dev/test/11/Double_attr->event_period: 1000
dev/test/11/String_spec_attr->event_period: 1000
dev/test/11/String_spec_attr->format: %s
dev/test/11/String_spec_attr->label: "Device label"
dev/test/11/String_spec_attr->unit: "Device unit"
dev/test/11/Long_attr_w->description: "Test description"
dev/test/11/Long_attr_w->display_unit: "Et ta soeur"
dev/test/11/Long_attr_w->event_period: 1000
dev/test/11/Long_attr_w->label: "Test label"
dev/test/11/Long_attr_w->max_alarm: 99999
dev/test/11/Long_attr_w->max_value: 100000
dev/test/11/Long_attr_w->min_alarm: 1
dev/test/11/Long_attr_w->min_value: 0
dev/test/11/Long_attr_w->standard_unit: 100
dev/test/11/Long_attr_w->unit: Kilogramme
dev/test/11/Event_change_tst->abs_change: 1
dev/test/11/Event_change_tst->event_period: 1000
dev/test/11/Event64_change_tst->abs_change: 1
dev/test/11/Event64_change_tst->event_period: 1000
dev/test/11/Encoded_attr->abs_change: 1
dev/test/11/Encoded_attr->event_period: 1000
dev/test/11/Encoded_image->event_period: 1000
dev/test/11/Encoded_image->format: RawImage
# DEVICE dev/test/12 properties 

dev/test/12->__SubDevices: dev/test/11

# DEVICE dev/test/12 attribute properties

dev/test/12/Double_attr->event_period: 1000
dev/test/12/String_spec_attr->event_period: 1000
dev/test/12/String_spec_attr->label: "Class label"
dev/test/12/String_spec_attr->unit: "Class unit"
dev/test/12/Long_attr_w->description: "Test description"
dev/test/12/Long_attr_w->display_unit: "Et ta soeur"
dev/test/12/Long_attr_w->event_period: 1000
dev/test/12/Long_attr_w->format: "Tres long"
dev/test/12/Long_attr_w->label: "Test label"
dev/test/12/Long_attr_w->max_alarm: 99999
dev/test/12/Long_attr_w->max_value: 100000
dev/test/12/Long_attr_w->min_alarm: 1
dev/test/12/Long_attr_w->min_value: 0
dev/test/12/Long_attr_w->standard_unit: gramme
dev/test/12/Long_attr_w->unit: Kilogramme
dev/test/12/Event64_change_tst->abs_change: 1
dev/test/12/Event64_change_tst->event_period: 1000
dev/test/12/Encoded_attr->abs_change: 1
dev/test/12/Encoded_attr->event_period: 1000
dev/test/12/Encoded_image->event_period: 1000
dev/test/12/Encoded_image->format: RawImage
