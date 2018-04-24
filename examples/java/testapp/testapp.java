// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

package testapp;

import com.nakasendo.Platform;
import com.nakasendo.PubKey;
import com.nakasendo.Key;
import com.nakasendo.KeyECSecp256k1;
import com.nakasendo.JniException;

public class testapp {      
    private static final Platform platform = Platform.create();         
    
    public static void main(String[] args) {    
        try {
            /* Sign and verify a message */
            
            // Get an arbitrary message for signing
            // (The contents are unimportant for this example)
            final String message = "Sample message for signing";
            
            // Get public and private keys
            Key key = KeyECSecp256k1.create();
            PubKey pubKey = key.getPubKey();
            
            // Sign the message using our private key
            final byte[] signature = key.sign(message.getBytes());
            
            // Verify the signed message using the corresponding public key
            if(pubKey.verify(message.getBytes(), signature)) {
                System.out.println("Signature verified ok");
            }           
        } catch (JniException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }
}

