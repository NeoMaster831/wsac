namespace WsACService.Layers.InterProcess;

public enum State
{
    NONE,
    READY
}

public class StateStore
{
    public State State { get; set; }
}
