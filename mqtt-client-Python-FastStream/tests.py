import pytest
from faststream.mqtt import TestMQTTBroker

import pub_sub_tcp
import pub_sub_tls


@pytest.mark.asyncio
async def test_tcp_pub_sub() -> None:
    async with TestMQTTBroker(pub_sub_tcp.broker):
        await pub_sub_tcp.publish_demo()

        pub_sub_tcp.on_message.mock.assert_called_once_with(
            {"message": "Hello from FastStream over TCP"}
        )


@pytest.mark.asyncio
async def test_tls_pub_sub() -> None:
    async with TestMQTTBroker(pub_sub_tls.broker):
        await pub_sub_tls.publish_demo()

        pub_sub_tls.on_message.mock.assert_called_once_with(
            {"message": "Hello from FastStream over TLS"}
        )
