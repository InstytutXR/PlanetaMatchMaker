﻿using System;
using PlanetaGameLabo.Serializer;

namespace PlanetaGameLabo.MatchMaker {
    [Serializable]
    public struct ClientAddress {
        [FixedLength(16)]
        public byte[] IPAddress;

        public ushort portNumber;
    }
}