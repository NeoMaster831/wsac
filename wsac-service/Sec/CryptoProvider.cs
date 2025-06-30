using System.Security.Cryptography;

namespace WsACService;

public class CryptoProvider
{
    private readonly ECDiffieHellman _dh;
    private byte[]? _sharedKey;

    public byte[] PublicKey { get; }
    
    public bool IsKeyExchanged => _sharedKey != null;

    public CryptoProvider()
    {
        _dh = ECDiffieHellman.Create(ECCurve.NamedCurves.nistP256);
        PublicKey = _dh.PublicKey.ExportSubjectPublicKeyInfo();
    }

    public void InstallClientKey(byte[] key)
    {
        var clientKey = CngKey.Import(key, CngKeyBlobFormat.EccPublicBlob);
        using var ecdhClient = new ECDiffieHellmanCng(clientKey);
        _sharedKey = _dh.DeriveKeyMaterial(ecdhClient.PublicKey);
    }

    public byte[] Encrypt(byte[] data)
    {
        if (_sharedKey == null)
            throw new InvalidOperationException("Key exchange is not completed.");

        using var aes = Aes.Create();
        aes.Key = _sharedKey;
        aes.GenerateIV();

        using var encryptor = aes.CreateEncryptor();
        var encrypted = encryptor.TransformFinalBlock(data, 0, data.Length);

        var result = new byte[aes.IV.Length + encrypted.Length];
        aes.IV.CopyTo(result, 0);
        encrypted.CopyTo(result, aes.IV.Length);

        return result;
    }

    public byte[] Decrypt(byte[] data)
    {
        if (_sharedKey == null)
            throw new InvalidOperationException("Key exchange is not completed.");

        using var aes = Aes.Create();
        aes.Key = _sharedKey;

        var iv = new byte[aes.BlockSize / 8];
        var encrypted = new byte[data.Length - iv.Length];

        Buffer.BlockCopy(data, 0, iv, 0, iv.Length);
        Buffer.BlockCopy(data, iv.Length, encrypted, 0, encrypted.Length);

        aes.IV = iv;

        using var decryptor = aes.CreateDecryptor();
        return decryptor.TransformFinalBlock(encrypted, 0, encrypted.Length);
    }
}
