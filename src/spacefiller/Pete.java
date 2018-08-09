package spacefiller;

import processing.core.PApplet;
import processing.core.PGraphics;
import processing.opengl.PShader;
import processing.serial.Serial;
import spacefiller.remote.Mod;

import java.util.Arrays;

public class Pete extends PApplet {
  public static void main(String[] args) {
    PApplet.main("spacefiller.Pete");
  }

  private static final String MAC_SERIAL_PORT = "/dev/tty.usbmodem1679111";
  private static final String WINDOWS_SERIAL_PORT = "COM4";

  PShader fluidPass;
  PShader displayPass;
  PGraphics fluidBuffer;
  PGraphics displayBuffer;
  float initialTime;
  AlgoplexController controller;

  @Mod(min = 0, max = 1)
  public float blur1 = 0.150183f;

  @Mod(min = 0, max = 1)
  public float comp = 0.6f;

  @Mod(min = 0, max = 1)
  public float something = 0.2f;

  @Override
  public void settings() {
    fullScreen(P3D);
  }

  @Mod
  public void resetTime() {
    initialTime = (float) (millis()/1000.0);
  }

  @Override
  public void setup() {
    println(Arrays.toString(Serial.list()));
    controller = new AlgoplexController(MAC_SERIAL_PORT, 9000);
    controller.register(this);
    controller.printAddresses();

    controller.controller(0).send(controller.target("/Pete/blur1"));
    controller.controller(1).send(controller.target("/Pete/comp"));
    controller.controller(2).send(controller.target("/Pete/something"));

    controller.controller(6).send(controller.target("/Pete/resetTime"));

    initialTime = (float) (millis()/1000.0);
    //size(400, 400, P3D);
    fullScreen(P3D);
    fluidBuffer = createGraphics(width/2,height/2,P2D);//width, height, P2D);
    fluidBuffer.noSmooth();
    displayBuffer = createGraphics(width/2,height/2,P2D);//width, height, P2D);
    displayBuffer.noSmooth();

    fluidPass = loadShader("strange-fluidFrag.glsl");
    displayPass = loadShader("displayFrag.glsl");

    fluidPass.set("resolution", (float) fluidBuffer.width, (float) fluidBuffer.height);
    displayPass.set("resolution", (float) displayBuffer.width, (float) displayBuffer.height);
  }

  @Override
  public void draw() {
    //background(0);
    fluidPass.set("time", (float) (millis()/1000.0-initialTime));
    displayPass.set("time", (float) (millis()/1000.0-initialTime));
    float x = map(mouseX, 0, width, 0, 1);
    float y = map(mouseY, 0, height, 1, 0);

    fluidPass.set("blur1", blur1);
    fluidPass.set("comp", comp);
    fluidPass.set("something", something);

    fluidPass.set("mouse", x, y);
    fluidBuffer.beginDraw();
    fluidBuffer.shader(fluidPass);
    fluidBuffer.rect(0, 0, fluidBuffer.width, fluidBuffer.height);
    fluidBuffer.endDraw();

    displayPass.set("mouse", x, y);
    displayBuffer.beginDraw();
    displayBuffer.shader(displayPass);
    displayBuffer.image(fluidBuffer, 0, 0);
    displayBuffer.endDraw();

    image(fluidBuffer, 0, 0, width, height);
  }
}
