namespace WsACService.Net.Models;

public enum FrameSignature : ushort
{
    // TODO : set values from cmake
    Test       = Config.TestSignature,
    Checkpoint = Config.CheckpointSignature,
    Regular    = Config.RegularSignature
}
