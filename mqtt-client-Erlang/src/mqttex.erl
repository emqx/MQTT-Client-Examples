%%-------------------------------------------------------------------------
%% Copyright (c) 2023 EMQ Technologies Co., Ltd. All Rights Reserved.
%%
%% Licensed under the Apache License, Version 2.0 (the "License");
%% you may not use this file except in compliance with the License.
%% You may obtain a copy of the License at
%%
%%     http://www.apache.org/licenses/LICENSE-2.0
%%
%% Unless required by applicable law or agreed to in writing, software
%% distributed under the License is distributed on an "AS IS" BASIS,
%% WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
%% See the License for the specific language governing permissions and
%% limitations under the License.
%%-------------------------------------------------------------------------

-module(mqttex).

-export([run/1]).

%-define(HOST, "127.0.0.1").
-define(HOST, "broker.emqx.io").

run(tcp) ->
    Client = connect_mqtt_broker(tcp, ?HOST, 1883),
    publish_example(Client),
    async_publish_example(Client),
    subscribe_example(Client),
    receive_message_example(Client),
    disconnect(Client);
run(tls) ->
    Client = connect_mqtt_broker(tls, ?HOST, 8883),
    publish_example(Client),
    async_publish_example(Client),
    subscribe_example(Client),
    receive_message_example(Client),
    disconnect(Client);
run(ws) ->
    Client = connect_mqtt_broker(ws, ?HOST, 8083),
    publish_example(Client),
    async_publish_example(Client),
    subscribe_example(Client),
    receive_message_example(Client),
    disconnect(Client);
run(wss) ->
    Client = connect_mqtt_broker(wss, ?HOST, 8084),
    publish_example(Client),
    async_publish_example(Client),
    subscribe_example(Client),
    receive_message_example(Client),
    disconnect(Client).

%%--------------------------------------------------------------------
%% Connect Examples

connect_mqtt_broker(Transport, Host, Port) ->
    connect_mqtt_broker(Transport, Host, Port, <<"mqtt-client-erl-tcp-example">>).

connect_mqtt_broker(Transport, Host, Port, ClientId)
  when Transport == tcp;
       Transport == tls ->
    Opts = base_options(Transport, Host, Port, ClientId),
    {ok, Client} = emqtt:start_link(Opts),
    case emqtt:connect(Client) of
        {ok, _Props} ->
            Client;
        {error, Reason} ->
            io:format("Failed to connect to broker~n"),
            error(Reason)
    end;
connect_mqtt_broker(Transport, Host, Port, ClientId)
  when Transport == ws;
       Transport == wss ->
    Opts = base_options(Transport, Host, Port, ClientId),
    {ok, Client} = emqtt:start_link(Opts),
    case emqtt:ws_connect(Client) of
        {ok, _Props} ->
            Client;
        {error, Reason} ->
            io:format("Failed to connect to broker~n"),
            error(Reason)
    end.

base_options(Transport, Host, Port, ClientId) ->
    Basic = [{host, Host},
             {port, Port},
             {clientid, ClientId},
             {username, <<"usera">>},
             {password, <<"pass">>},
             {clean_start, true},
             {keepalive, 60}
            ],
    case Transport == tls orelse Transport == wss of
        true ->
            WSTransportOptions = 
                   [{ssl, true},
                    {ssl_opts, [
                         {cacertfile, "certs/cacert.pem"},
                         {certfile, "certs/client-cert.pem"},
                         {keyfile, "certs/client-key.pem"},
                         %% Don't verify the server's pem
                         {verify, verify_none}
                         %% or enable verify the server's cert
                         %{verify, verify_peer}
                        ]
                    }]
            ++ 
            case Transport == wss of
                true ->
                    [
                        { ws_transport_options, [
                            {transport, tls},
                            {protocols,[http]}
                        ]}
                    ];
                _ ->
                    []
            end,
            WSTransportOptions ++ Basic;
        _ ->
            Basic
    end.

%%--------------------------------------------------------------------
%% Pub/Sub Examples

%% Pub/Sub examples
publish_example(Client) ->
    Topic = <<"testtopic/foo">>,
    %% publish qos0 msg
    ok = emqtt:publish(
           Client,
           Topic,
           <<"[1] Hi, this is payload content">>,
           0
          ),
    io:format("publish msg[1] success!~n"),
    %% publish qos1 msg
    {ok, _ReturnCode1} =
        emqtt:publish(
          Client,
          Topic,
          <<"[2] Hi, this is payload content">>,
          1
          ),
    io:format("publish msg[2] success!~n"),
    %% publish qos2 msg
    {ok, _ReturnCode2} =
        emqtt:publish(
          Client,
          Topic,
          <<"[3] Hi, this is payload content">>,
          2
          ),
    io:format("publish msg[3] success!~n"),
    ok.

%% Async publish examples
async_publish_example(Client) ->
    Topic = <<"testtopic/bar">>,
    %% publish qos0 msg
    ok = emqtt:publish_async(
           Client,
           Topic,
           <<"[1] Hi, this msg is from async publish">>,
           {fun(Result) ->
                    %% assert
                    ok = Result,
                    io:format("async publish msg[1] success~n")
            end, []}
          ),
    %% publish qos1 msg
    ok = emqtt:publish_async(
           Client,
           Topic,
           <<"[2] Hi, this msg is from async publish">>,
           1,
           {fun(Result) ->
                    %% assert
                    {ok, _} = Result,
                    io:format("async publish msg[2] success~n")
            end, []}
          ),
    %% publish qos2 msg
    Caller = self(),
    ok = emqtt:publish_async(
           Client,
           Topic,
           <<"[3] Hi, this msg is from async publish">>,
           2,
           {fun(Result) ->
                    %% assert
                    {ok, _} = Result,
                    io:format("async publish msg[3] success~n"),
                    Caller ! done
            end, []}
          ),
    %% waiting for all publish msg completed
    receive
        done -> ok
    after 5000 -> error(timeout)
    end.

%% Subscribe
subscribe_example(Client) ->
    %% subscribe
    {ok, _, _} = emqtt:subscribe(Client, <<"testtopic/0">>, 0),
    {ok, _, _} = emqtt:subscribe(Client, <<"testtopic/1">>, 1),
    {ok, _, _} = emqtt:subscribe(Client, <<"testtopic/2">>, 2),
    %% unsubscribe
    {ok, _, _} = emqtt:unsubscribe(Client, <<"testtopic/0">>),
    {ok, _, _} = emqtt:unsubscribe(Client, <<"testtopic/1">>),
    {ok, _, _} = emqtt:unsubscribe(Client, <<"testtopic/2">>),
    io:format("subscribe/unsubscribe success~n"),
    ok.

%%--------------------------------------------------------------------
%% Receive messages

receive_message_example(Client) ->
    Topic = <<"testtopic/1">>,
    Publisher = connect_mqtt_broker(tcp, ?HOST, 1883, <<"publisher">>),
    %% subsribe first
    {ok, _, _} = emqtt:subscribe(Client, Topic, 1),
    ok = emqtt:publish(Publisher, Topic, <<"This is message from publisher">>),
    io:format("publisher publish success~n"),
    receive
        {publish, _Msg = #{topic := RecvTopic, payload := Payload}} ->
            io:format("received message topic: ~p, payload: ~p~n",
                      [RecvTopic, Payload])
    after 5000 ->
          error(timeout)
    end,
    disconnect(Publisher),
    ok.


%%--------------------------------------------------------------------
%% Disconnect

disconnect(Client) ->
    ok = emqtt:disconnect(Client).
