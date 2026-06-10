idf_component_register(
    SRCS
        "src/main.c"
        "src/button.c"
        "src/led.c"
    INCLUDE_DIRS
        "include"
)

# For automous test of components: changne SRCS on button/button.c button/button_test.c
# and also add: target_compile_definitions(${COMPONENT_LIB} PRIVATE BUTTON_TEST)

# Uncomment for testing traffic lights
# idf_component_register(
#     SRCS
#         "src/led.c"         # led.c -> button.c
#         "src/led_test.c"    # led_test.c -> button_test.c
#     INCLUDE_DIRS
#         "include"
# )
# target_compile_definitions(${COMPONENT_LIB} PRIVATE TRAFFIC_TEST)

# Uncomment for testing button
# idf_component_register(
#     SRCS
#         "src/button.c"         # led.c -> button.c
#         "src/button_test.c"    # led_test.c -> button_test.c
#     INCLUDE_DIRS
#         "include"
# )
# target_compile_definitions(${COMPONENT_LIB} PRIVATE BUTTON_TEST)