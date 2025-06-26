namespace WsACService;

public sealed class PipeManager
{
    public const int NumThread = 4;

    private bool _init;
    private readonly ManualResetEventSlim _cv = new(false);
    
    public PipeServer[] Workers { get; } = new PipeServer[NumThread];

    public void NotifyInit()
    {
        _init = true;
        _cv.Set();
    }
    
    public bool WaitForInit(CancellationToken ct)
    {
        if (_init)
            return true;
        
        _cv.Wait(ct);
        return _init;
    }
}
