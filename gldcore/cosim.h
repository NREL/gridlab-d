
/********************************************************************/
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include "output.h"
#include "object.h"
#include "exception.h"
#include "random.h"
#include "globals.h"
#include "math.h"
#include "time.h"

// multithreading
#include "omp.h"
#include "pthread.h"

//  bus and mesage formats
#include "zmq.h"
#include "jansson.h"

//  stuff from server code
// JSON Parser Functions

/********************************************************************/
void * subscription_thread();
void * subscription_thread_object_bus();

// ZMQ bus publish and subscribe connections
void *responder;
void *requester;
void *responderobj;
void *requesterobj;



json_t *load_json(const char *text);
void json_get_list(json_t *root);
void json_set_list(json_t *root);
void json_control(json_t *root);
/********************************************************************/
/******convert a json string to a json object ******/
json_t *load_json(const char *text)
{
    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

    if (root) {
        return root;
    } else {
        fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
        return (json_t *)0;
    }
}
/****************************************************************************************/

/****** Retuns a list of JSON objects over the ZMQ message bus ******/
void json_get_list(json_t *root)
{

    static int flag_array = 0;
    char gridlabd_response[2048] = "";
    switch(json_typeof(root))
    {
      case  JSON_OBJECT :{
          printf("\n type : JSON_OBJECT");
          const char *key;
          json_t *value;
          OBJECT *obj;
          char property_name[200];
          char object_name[200];
          char property_value[200];


          json_object_foreach(root, key, value) {

                strcpy (property_name,json_string_value(json_object_get(root,"name")) );
                strcpy (object_name,json_string_value(json_object_get(root,"key")) );
                if (strcmp(object_name,"global") == 0)
                {
                  // global_setvar(property_name,property_value);
                  strcpy(property_value,"");
                  if (global_getvar(property_name,property_value,sizeof(property_value))==NULL)
               		{
               			output_error("global variable '%s' not found", property_name);
               			return ;
               		}
                  output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
                  obj = NULL;
                  key = NULL;
                  value = NULL;
                  return;
                }


                else
                {
                obj = object_find_name(object_name);
                if (obj == NULL)
            			output_error("no such obkect %s",object_name);
                char buffer[1024]="";
                if ( !object_get_value_by_name(obj,property_name,buffer,sizeof(buffer)))
            		{
            			output_error("object '%s' property '%s' not found ", object_name, property_name);
            			return ;
            		}
                json_object_set(root,"value",json_string(buffer));

                if (flag_array == 0)
                {
                  output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));

                  strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);

                }

          }
          obj = NULL;
          key = NULL;
          value = NULL;
        }
      break;}
      case  JSON_ARRAY :{

        flag_array = 1;
        printf("\n type : JSON_ARRAY");
        size_t index;
        json_t *value;
        json_array_foreach(root,index,value)
        {
          json_get_list(value);
        }
        output_verbose("\n json array : %s",json_dumps(root,JSON_ENCODE_ANY));
        strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
        strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
        zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
        flag_array =0;

      break;}
      case  JSON_NULL :{ printf("\n type : JSON_NULL");break;}
    }

}
/****************************************************************************************/

/****** Sets a list of JSON objects, Retuns a list of
 JSON objects over the ZMQ message bus ******/
void json_set_list(json_t *root)
{
  char gridlabd_response[2048] = "";
    static int flag_array = 0;
    switch(json_typeof(root))
    {
      case  JSON_OBJECT :{
          printf("\n type : JSON_OBJECT");
          const char *key;
          json_t *value;
          OBJECT *obj;
          char property_name[200];
          char object_name[200];
          // char property_value[200];
          char property_value[200];
          char gridlabd_response[2048] = "";

          json_object_foreach(root, key, value) {

                strcpy (property_name,json_string_value(json_object_get(root,"name")) );
                strcpy (object_name,json_string_value(json_object_get(root,"key")) );
                strcpy (property_value,json_string_value(json_object_get(root,"value")) );

                if (strcmp(object_name,"global") == 0)
                {
                  global_setvar(property_name,property_value);
                  strcpy(property_value,"");
                  if (global_getvar(property_name,property_value,sizeof(property_value))==NULL)
               		{
               			output_error("global variable '%s' not found", property_name);
               			return ;
               		}
                  output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
                  obj = NULL;
                  key = NULL;
                  value = NULL;
                  return;
                }

                else{

                obj = object_find_name(object_name);
                if (obj == NULL)
            			output_error("no such obkect %s",object_name);
                char buffer[1024]="";


                if (!object_set_value_by_name(obj,property_name,property_value))
                {
                  output_error("object '%s' property '%s' can't be set'", object_name, property_name);
                  return ;
                }

                if ( !object_get_value_by_name(obj,property_name,buffer,sizeof(buffer)))
            		{
            			output_error("object '%s' property '%s' not found ", object_name, property_name);
            			return ;
            		}
                json_object_set(root,"value",json_string(buffer));

                if (flag_array == 0)
                {
                  output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);

                }


          }
          obj = NULL;
      		key = NULL;
      		value = NULL;
        }

      break;}
      case  JSON_ARRAY :{

        flag_array = 1;
        printf("\n type : JSON_ARRAY");
        size_t index;
        json_t *value;
        json_array_foreach(root,index,value)
        {
          json_set_list(value);
        }
        output_verbose("\n json array : %s",json_dumps(root,JSON_ENCODE_ANY));
        strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
        strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
        zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
        flag_array =0;

      break;}
      case  JSON_NULL :{ printf("\n type : JSON_NULL");break;}
    }

}
/****************************************************************************************/

/****** Resceives control signals over ZMQ message bus ******/
void json_control(json_t *root)
{
  char gridlabd_response[2048] = "";

    switch(json_typeof(root))
    {
      case  JSON_OBJECT :{
          printf("\n type : JSON_OBJECT");
          const char *key;
          json_t *value;

          char property_name[200];
          char object_name[200];
          char property_value[200];


          json_object_foreach(root, key, value) {

                strcpy (property_name,json_string_value(json_object_get(root,"name")) );
                strcpy (object_name,json_string_value(json_object_get(root,"key")) );
                strcpy (property_value,json_string_value(json_object_get(root,"value")) );

                if (strcmp(object_name,"control")==0)
                {
                    if (strcmp(property_name,"pauseat") == 0)
                    {
                      TIMESTAMP ts;
                      ts = convert_to_timestamp(property_value);
                      if ( ts!=TS_INVALID )
                      {
                      exec_mls_resume(ts);
                      }

                }

                  else if (strcmp(property_name,"stop") == 0)
                  {
                      global_stoptime = global_clock;

                  }
                  else if (strcmp(property_name,"shutdown") == 0)
                  {
                      exit(XC_SUCCESS);

                  }
                  else if (strcmp(property_name,"resume") == 0)
                  {
                      exec_mls_resume(TS_NEVER);

                  }

                }

          }

      		key = NULL;
      		value = NULL;

      break;}

      case  JSON_NULL :{ printf("\n  Not a control signal type");break;}
    }

}
/****************************************************************************************/

/****** starts up the ZMQ message bus ******/
void * subscription_thread()
{
  //  Scubscribe heleprs
  void *context = zmq_ctx_new();
  responder = zmq_socket(context, ZMQ_SUB);
  int rc = zmq_bind(responder, "tcp://*:5548");
  int aa = zmq_setsockopt( responder, ZMQ_SUBSCRIBE, "", 0 );

  //  publish helpers
  void *context2 = zmq_ctx_new();
  requester = zmq_socket(context2, ZMQ_PUB);
  int rc2 = zmq_bind(requester, "tcp://*:5648");
  double time_spent ;
  clock_t end ;
  clock_t begin ;

  printf("Receiver: Started\n");

  char buffer[2048];
  char* put_data = (char*)malloc(2048);
    while (1)
    {
          int num = zmq_recv(responder, buffer, 2048, 0);
      begin = clock();
          if (num > 0)
          {
                buffer[num] = '\0';
                if (strstr(buffer, "GRIDLABD_REQUEST") != NULL)
                {

                  output_fatal("\n received %s ",buffer);
                  sprintf(put_data,buffer);
                  put_data = strchr(put_data,'$');
                  put_data++;
                      if (strstr(buffer, "GRIDLABD_REQUEST_GET") != NULL)
                      {


                          json_t *root = load_json(put_data);
                          if (root)
                          {
                              json_get_list(root);
                              json_decref(root);
                          }

                      }
                      else if (strstr(buffer, "GRIDLABD_REQUEST_SET") != NULL)
                      {


                          json_t *root = load_json(put_data);
                          if (root)
                          {
                            json_set_list(root);
                            json_decref(root);
                          }


                      }

                      else if (strstr(buffer, "GRIDLABD_REQUEST_CONTROL") != NULL)
                      {


                          json_t *root = load_json(put_data);
                          if (root)
                          {
                            json_control(root);
                            json_decref(root);
                          }


                      }



                }

          }
      end = clock();
      time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
      output_fatal("Time to service a zmq request : %lf ", time_spent);
        // sleep(0.1);

    }

    zmq_close(responder);
    zmq_ctx_destroy(context);
    zmq_close(requester);
    zmq_ctx_destroy(context2);
    free(*put_data);

}

/****************************************************************************************/

/****************************************************************************************/

/****** starts up the ZMQ message bus for objects ******/
void * subscription_thread_object_bus()
{
  //  Scubscribe heleprs
  void *context = zmq_ctx_new();
  responderobj = zmq_socket(context, ZMQ_SUB);
  int rc = zmq_bind(responderobj, "tcp://*:5248");
  int aa = zmq_setsockopt( responderobj, ZMQ_SUBSCRIBE, "", 0 );

  //  publish helpers
  void *context2 = zmq_ctx_new();
  requesterobj = zmq_socket(context2, ZMQ_PUB);
  int rc2 = zmq_bind(requesterobj, "tcp://*:5348");

  printf("Object Bus: Started\n");

  char buffer[2048];

    while (1)
    {
          int num = zmq_recv(responderobj, buffer, 2048, 0);

          if (num > 0)
          {
                      zmq_send(requesterobj,buffer, strlen(buffer), 0);
          }



        // sleep(0.1);

    }

    zmq_close(responderobj);
    zmq_ctx_destroy(context);
    zmq_close(requesterobj);
    zmq_ctx_destroy(context2);

}


/****************************************************************************************/
