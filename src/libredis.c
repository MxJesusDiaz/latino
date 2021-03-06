/*
The MIT License (MIT)

Copyright (c) 2015 - 2016. Latino

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "latmv.h"

#ifndef _WIN32
#include<stdlib.h>

#include "hiredis.h"
#include "latino.h"
#include "latgc.h"

#define LIB_REDIS_NAME "redis"

void lat_redis_conectar(lat_mv *mv) {
  lat_objeto *puerto = lat_desapilar(mv);
  lat_objeto *servidor = lat_desapilar(mv);
  redisContext *redis;
  const char *servidor2 = __cadena(servidor);
  int puerto2 = __numerico(puerto);
  struct timeval timeout = {1, 500000}; // 1.5 segundos
  redis = redisConnectWithTimeout(servidor2, puerto2, timeout);
  if (redis == NULL || redis->err) {
    if (redis) {
      redisFree(redis);
      lat_error("Linea %d, %d: %s", servidor->num_linea,
                      servidor->num_columna, "error en conexión.");
    } else {
      lat_error("Linea %d, %d: %s", servidor->num_linea,
                      servidor->num_columna, "error en contexto redis.");
    }
  }
  //encapsulo el dato (struct) de redis
  lat_objeto *cref = lat_cdato_nuevo(mv, redis);
  lat_apilar(mv, cref);
}

void lat_redis_liberar(lat_mv *mv) {
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisFree(conexion);
  //lat_gc_agregar(mv, o);
}

void lat_redis_ping(lat_mv *mv) {
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta = redisCommand(conexion, "PING");
  if (!respuesta->str) {
    lat_error("Error: error al obtener respuesta de redis");
  }
  lat_objeto *tmp = lat_cadena_nueva(mv, strdup(respuesta->str));
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

void lat_redis_asignar(lat_mv *mv) {
  lat_objeto *cadena = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta =
      redisCommand(conexion, "SET %s %s", __cadena(llave), __cadena(cadena));
  if (!respuesta->str) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error asignar llave.");
  }
  freeReplyObject(respuesta);
}

void lat_redis_hasignar(lat_mv *mv) {
  lat_objeto *cadena = lat_desapilar(mv);
  lat_objeto *llave2 = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta = redisCommand(conexion, "HSET %s %s %s", __cadena(llave),
                           __cadena(llave2), __cadena(cadena));
  freeReplyObject(respuesta);
}

void lat_redis_obtener(lat_mv *mv) {
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta = redisCommand(conexion, "GET %s", __cadena(llave));
  if (!respuesta->str) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error en obtener llave.");
  }
  lat_objeto * tmp = lat_cadena_nueva(mv, strdup(respuesta->str));
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

void lat_redis_hobtener(lat_mv *mv) {
  lat_objeto *llave2 = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta =
      redisCommand(conexion, "HGET %s %s", __cadena(llave), __cadena(llave2));
  if (!respuesta->str) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error en obtener llave.");
  }
  lat_objeto *tmp = lat_cadena_nueva(mv, strdup(respuesta->str));
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

void lat_redis_borrar(lat_mv *mv) {
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisCommand(conexion, "DEL %s", __cadena(llave));
}

void lat_redis_hborrar(lat_mv *mv) {
  lat_objeto *llave2 = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisCommand(conexion, "HDEL %s %s", __cadena(llave), __cadena(llave2));
}

void lat_redis_aumentar(lat_mv *mv) {
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta = redisCommand(conexion, "INCR %s", __cadena(llave));
  if (!respuesta->integer) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error al incrementar el entero.");
  }
  lat_objeto *tmp = lat_numerico_nuevo(mv, (double)respuesta->integer);
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

void lat_redis_haumentar(lat_mv *mv) {
  lat_objeto *llave2 = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  respuesta = redisCommand(conexion, "HINCRBY %s %s 1", __cadena(llave),
                           __cadena(llave2));
  if (!respuesta->integer) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error al incrementar el entero.");
  }
  lat_objeto *tmp = lat_numerico_nuevo(mv, (double)respuesta->integer);
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

void lat_redis_incrementar(lat_mv *mv) {
  lat_objeto *numero = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  long int numerico = __numerico(numero);
  respuesta = redisCommand(conexion, "INCRBY %s %i", __cadena(llave), numerico);
  if (!respuesta->integer) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error al incrementar el entero.");
  }
  lat_objeto *tmp = lat_numerico_nuevo(mv, respuesta->integer);
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

void lat_redis_hincrementar(lat_mv *mv) {
  lat_objeto *numero = lat_desapilar(mv);
  lat_objeto *llave2 = lat_desapilar(mv);
  lat_objeto *llave = lat_desapilar(mv);
  lat_objeto *o = lat_desapilar(mv);
  redisContext *conexion = __cdato(o);
  redisReply *respuesta;
  long int numerico = __numerico(numero);
  respuesta = redisCommand(conexion, "HINCRBY %s %s %i", __cadena(llave),
                           __cadena(llave2), numerico);
  if (!respuesta->integer) {
    lat_error("Linea %d, %d: %s", llave->num_linea, llave->num_columna,
                    "error al incrementar el entero.");
  };
  lat_objeto *tmp = lat_numerico_nuevo(mv, respuesta->integer);
  lat_apilar(mv, tmp);
  //lat_gc_agregar(mv, tmp);
  freeReplyObject(respuesta);
}

static const lat_CReg lib_redis[] = {
    {"conectar", lat_redis_conectar, 2},
    {"liberar", lat_redis_liberar, 1},
    {"ping", lat_redis_ping, 1},
    {"asignar", lat_redis_asignar, 3},
    {"hasignar", lat_redis_hasignar, 4},
    {"obtener", lat_redis_obtener, 2},
    {"hobtener", lat_redis_hobtener, 3},
    {"borrar", lat_redis_borrar, 2},
    {"hborrar", lat_redis_hborrar, 3},
    {"aumentar", lat_redis_aumentar, 2},
    {"haumentar", lat_redis_haumentar, 3},
    {"incrementar", lat_redis_incrementar, 3},
    {"hincrementar", lat_redis_hincrementar, 4},
    {NULL, NULL}};

#endif

void lat_importar_lib_redis(lat_mv *mv) {
#ifndef _WIN32
  lat_importar_lib(mv, LIB_REDIS_NAME, lib_redis);
#endif
}
