using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;

namespace WsACService.Net.Models;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FrameHeader
{
    public readonly     FrameSignature Signature;
    public readonly     long           DataSize;
    public unsafe fixed byte           Nonce[12];
    public unsafe fixed byte           MAC[16];

    public FrameHeader(FrameSignature signature, long size = 0)
    {
        Signature = signature;
        DataSize  = size;

        unsafe
        {
            fixed (byte* pNonce = Nonce)
            {
                var nonce = new Span<byte>(pNonce, 12);
                RandomNumberGenerator.Fill(nonce);
            }
        }

        SignUnsafe();
    }

    public override string ToString()
    {
        static void AppendHex(StringBuilder builder, Span<byte> bytes)
        {
            for (var i = 0; i < bytes.Length; ++i)
            {
                if (i != 0)
                    builder.Append(':');
                builder.Append(bytes[i].ToString("X2"));
            }
        }

        var builder = new StringBuilder()
            .Append("Frame{ ")
            .Append("Signature = ").Append(Signature.ToString()).Append(", ")
            .Append("DataSize = ").Append(DataSize).Append(", ");

        builder.Append("Nonce = ");
        unsafe
        {
            fixed (byte* pNonce = Nonce)
            {
                var nonce = new Span<byte>(pNonce, 12);
                AppendHex(builder, nonce);
            }
        }

        builder.Append(", MAC = ");
        unsafe
        {
            fixed (byte* pMAC = MAC)
            {
                var mac = new Span<byte>(pMAC, 16);
                AppendHex(builder, mac);
            }
        }

        builder.Append('}');

        return builder.ToString();
    }

    private void SignUnsafe()
    {
        Span<byte> buffer = stackalloc byte[10];
        MemoryMarshal.Write(buffer.Slice(0, 2), in Signature);
        MemoryMarshal.Write(buffer.Slice(2, 8), in DataSize);

        unsafe
        {
            fixed (byte* pNonce = Nonce)
            fixed (byte* pMAC = MAC)
            {
                var nonce = new Span<byte>(pNonce, 12);
                var mac   = new Span<byte>(pMAC, 16);

                using var cipher = new ChaCha20Poly1305(Config.PSK[0]);
                cipher.Encrypt(nonce, ReadOnlySpan<byte>.Empty, Span<byte>.Empty, mac, buffer);
            }
        }
    }

    public FrameHeader Sign()
    {
        var copy = this;
        copy.SignUnsafe();
        return copy;
    }

    public bool Validate()
    {
        unsafe
        {
            var signed = Sign();
            fixed (byte* pMAC = MAC)
            {
                var mac  = new Span<byte>(pMAC, 16);
                var oMAC = new Span<byte>(signed.MAC, 12);

                return mac.SequenceEqual(oMAC);
            }
        }
    }
}