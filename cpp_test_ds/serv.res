DevTest/api/DEVICE/DevTest: "dev/test/10",\
                            "dev/test/11",\
                            "dev/test/12"

#############################################
# CLASS DevTest

CLASS/DevTest->property1: "property test 1",\
                          "property test 1"
CLASS/DevTest->property2: "property test 2",\
                          "property test 2",\
                          "line 2",\
                          "line 2",\
                          "lien 3",\
                          "lien 3"

# CLASS DevTest attribute properties

CLASS/DevTest/String_spec_attr->format: "Class format"
CLASS/DevTest/String_spec_attr->label: "Class label"
CLASS/DevTest/String_spec_attr->unit: "Class unit"
CLASS/DevTest/Added_short_attr->label: "From db (class)"


# DEVICE dev/test/10 properties 

dev/test/10->non_auto_polled_attr: short_ima_attr,\
                                  event_quality_tst,\
                                  status,\
                                  state,\
                                  short_attr,\
                                  event_change_tst
dev/test/10->poll_old_factor: 4
dev/test/10->poll_ring_depth: 10

# DEVICE dev/test/10 attribute properties

dev/test/10/Short_attr->event_period: 600
dev/test/10/Short_attr->format: 27113
dev/test/10/Long_attr->max_alarm: 1500,\
                                  1500
dev/test/10/Long_attr->min_alarm: 1000,\
                                  1000
dev/test/10/String_attr->format: %s
dev/test/10/Short_attr_rw->display_unit: 27133
dev/test/10/Short_attr_rw->format: 27133
dev/test/10/Short_attr_w->__value: 345
dev/test/10/String_attr_w->format: %c,\
                                   %c
dev/test/10/String_attr_w->__value: "Do you want to dance?"
dev/test/10/Event_change_tst->abs_change: 1
dev/test/10/Event_change_tst->archive_abs_change: 1
dev/test/10/Boolean_attr_w->__value: false
dev/test/10/Float_spec_attr_rw->1: 15.5
dev/test/10/Float_spec_attr_rw->3.5: max_value
# DEVICE dev/test/11 properties 

dev/test/11->non_auto_polled_attr: String_attr

# DEVICE dev/test/11 attribute properties

dev/test/11/String_spec_attr->format: "Device format"
dev/test/11/String_spec_attr->label: "Device label"
dev/test/11/String_spec_attr->unit: "Device unit"
# DEVICE dev/test/12 properties 

dev/test/12->non_auto_polled_attr: String_attr

# DEVICE dev/test/12 attribute properties

dev/test/12/Long_attr_w->description: "Test description",\
                                      "Test description"
dev/test/12/Long_attr_w->display_unit: "Et ta soeur",\
                                       "Et ta soeur"
dev/test/12/Long_attr_w->format: "Tres long",\
                                 "Tres long"
dev/test/12/Long_attr_w->label: "Test label",\
                                "Test label"
dev/test/12/Long_attr_w->max_alarm: 99999,\
                                    99999
dev/test/12/Long_attr_w->max_value: 100000,\
                                    100000
dev/test/12/Long_attr_w->min_alarm: 1,\
                                    1
dev/test/12/Long_attr_w->min_value: 0,\
                                    0
dev/test/12/Long_attr_w->standard_unit: gramme,\
                                        gramme
dev/test/12/Long_attr_w->unit: Kilogramme,\
                               Kilogramme
dev/test/12/Event_change_tst->abs_change: 1
