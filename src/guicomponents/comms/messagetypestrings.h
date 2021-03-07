#pragma once
#include <guicomponents/comms/comms_export.h>
#include <guicomponents/comms/messagetypes.h>


namespace ngs {

COMMS_EXPORT const char* receiver_type_to_string(ReceiverType t);
COMMS_EXPORT const char* web_driver_request_type_to_string(WebDriverRequestType t);
COMMS_EXPORT const char* chrome_request_type_to_string(ChromeRequestType t);
COMMS_EXPORT const char* platform_request_type_to_string(PlatformRequestType t);
COMMS_EXPORT const char* firebase_request_type_to_string(FirebaseRequestType t);
COMMS_EXPORT const char* mouse_action_type_to_string(MouseActionType t);
COMMS_EXPORT const char* text_action_type_to_string(TextActionType t);
COMMS_EXPORT const char* element_action_type_to_string(ElementActionType t);
COMMS_EXPORT const char* info_type_to_string(InfoType t);
COMMS_EXPORT const char* message_type_to_string(MessageType t);
COMMS_EXPORT const char* element_type_to_string(ElementType t);
COMMS_EXPORT const char* direction_type_to_string(DirectionType t);
COMMS_EXPORT const char* http_send_type_to_string(HTTPSendType t);
COMMS_EXPORT const char* license_edition_to_string(LicenseEdition t);

}
