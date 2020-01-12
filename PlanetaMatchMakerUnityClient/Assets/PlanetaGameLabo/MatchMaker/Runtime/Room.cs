namespace PlanetaGameLabo.MatchMaker
{
    public interface IRoomInfo
    {
        byte roomGroupIndex { get; }
        uint roomId { get; }
        PlayerFullName hostPlayerFullName { get; }
        RoomSettingFlag settingFlags { get; }
        byte maxPlayerCount { get; }
        byte currentPlayerCount { get; }
        Datetime createDatetime { get; }
    }

    public interface IHostingRoomInfo
    {
        byte roomGroupIndex { get; }
        uint roomId { get; }
        byte maxPlayerCount { get; }
        bool isPublic { get; }
    }

    public interface IRoomGroupInfo
    {
        string name { get; }
    }
}