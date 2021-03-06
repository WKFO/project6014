/*
 Copyright (C) 2011 Joris van de Donk

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.

 Joris van de Donk - joris@mooses.nl
 */
package uqmstarmap;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;

public class MapShow extends javax.swing.JFrame {

    StarmapManager pcr;

    /**
     * Creates new form MapShow
     */
    public MapShow() {
        initComponents();
        pcr = new StarmapManager();
        updateStatusBar();
    }

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jButton1 = new javax.swing.JButton();
        jPanel1 = new javax.swing.JPanel();
        canvas2 = new java.awt.Canvas();
        jLabel1 = new javax.swing.JLabel();
        jToolBar1 = new javax.swing.JToolBar();
        jLabel2 = new javax.swing.JLabel();
        jSeparator1 = new javax.swing.JToolBar.Separator();
        jLabel3 = new javax.swing.JLabel();
        jSeparator2 = new javax.swing.JToolBar.Separator();
        jLabel4 = new javax.swing.JLabel();
        jMenuBar1 = new javax.swing.JMenuBar();
        jMenu1 = new javax.swing.JMenu();
        jMenuItem7 = new javax.swing.JMenuItem();
        jSeparator3 = new javax.swing.JPopupMenu.Separator();
        jMenuItem1 = new javax.swing.JMenuItem();
        jMenuItem8 = new javax.swing.JMenuItem();
        jMenu2 = new javax.swing.JMenu();
        jMenuItem2 = new javax.swing.JMenuItem();
        jMenuItem9 = new javax.swing.JMenuItem();
        jMenuItem4 = new javax.swing.JMenuItem();
        jMenuItem3 = new javax.swing.JMenuItem();
        jMenu5 = new javax.swing.JMenu();
        jMenuItem11 = new javax.swing.JMenuItem();
        jMenu3 = new javax.swing.JMenu();
        jMenuItem5 = new javax.swing.JMenuItem();
        jMenuItem6 = new javax.swing.JMenuItem();
        jMenu4 = new javax.swing.JMenu();
        jMenuItem10 = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        setTitle("MapConstellationEditor");

        jButton1.setText("Repaint!");
        jButton1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jButton1ActionPerformed(evt);
            }
        });

        jPanel1.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentResized(java.awt.event.ComponentEvent evt) {
                jPanel1ComponentResized(evt);
            }
        });

        canvas2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                canvas2MouseClicked(evt);
            }
            public void mousePressed(java.awt.event.MouseEvent evt) {
                canvas2MousePressed(evt);
            }
            public void mouseReleased(java.awt.event.MouseEvent evt) {
                canvas2MouseReleased(evt);
            }
        });
        canvas2.addComponentListener(new java.awt.event.ComponentAdapter() {
            public void componentResized(java.awt.event.ComponentEvent evt) {
                canvas2ComponentResized(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(canvas2, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 931, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(canvas2, javax.swing.GroupLayout.DEFAULT_SIZE, 600, Short.MAX_VALUE)
        );

        jLabel1.setText("...");

        jToolBar1.setFloatable(false);
        jToolBar1.setRollover(true);

        jLabel2.setText("N Starnames loaded (BUILT_IN)");
        jToolBar1.add(jLabel2);
        jToolBar1.add(jSeparator1);

        jLabel3.setText("N Stars");
        jToolBar1.add(jLabel3);
        jToolBar1.add(jSeparator2);

        jLabel4.setText("N Constellation Lines");
        jToolBar1.add(jLabel4);

        jMenu1.setText("File");

        jMenuItem7.setText("Load & Parse main .map");
        jMenuItem7.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem7ActionPerformed(evt);
            }
        });
        jMenu1.add(jMenuItem7);
        jMenu1.add(jSeparator3);

        jMenuItem1.setText("Load & Parse plandata.c");
        jMenuItem1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem1ActionPerformed(evt);
            }
        });
        jMenu1.add(jMenuItem1);

        jMenuItem8.setText("Load Starnames .txt file");
        jMenuItem8.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem8ActionPerformed(evt);
            }
        });
        jMenu1.add(jMenuItem8);

        jMenuBar1.add(jMenu1);

        jMenu2.setText("View");

        jMenuItem2.setText("Stars");
        jMenuItem2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem2ActionPerformed(evt);
            }
        });
        jMenu2.add(jMenuItem2);

        jMenuItem9.setText("Starnames");
        jMenuItem9.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem9ActionPerformed(evt);
            }
        });
        jMenu2.add(jMenuItem9);

        jMenuItem4.setText("Stars (MapTool format)");
        jMenuItem4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem4ActionPerformed(evt);
            }
        });
        jMenu2.add(jMenuItem4);

        jMenuItem3.setText("Constellations (MapTool format)");
        jMenuItem3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem3ActionPerformed(evt);
            }
        });
        jMenu2.add(jMenuItem3);

        jMenuBar1.add(jMenu2);

        jMenu5.setText("Tools");

        jMenuItem11.setText("Save as .dia file");
        jMenuItem11.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem11ActionPerformed(evt);
            }
        });
        jMenu5.add(jMenuItem11);

        jMenuBar1.add(jMenu5);

        jMenu3.setText("Old untested stuff");

        jMenuItem5.setText("Save XML");
        jMenuItem5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem5ActionPerformed(evt);
            }
        });
        jMenu3.add(jMenuItem5);

        jMenuItem6.setText("Import XML");
        jMenuItem6.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem6ActionPerformed(evt);
            }
        });
        jMenu3.add(jMenuItem6);

        jMenuBar1.add(jMenu3);

        jMenu4.setText("Help");

        jMenuItem10.setText("Help");
        jMenuItem10.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                jMenuItem10ActionPerformed(evt);
            }
        });
        jMenu4.add(jMenuItem10);

        jMenuBar1.add(jMenu4);

        setJMenuBar(jMenuBar1);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jButton1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel1, javax.swing.GroupLayout.DEFAULT_SIZE, 847, Short.MAX_VALUE)))
                .addContainerGap())
            .addComponent(jToolBar1, javax.swing.GroupLayout.DEFAULT_SIZE, 955, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jButton1)
                    .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jToolBar1, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void jButton1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jButton1ActionPerformed
        repaintMap();
    }//GEN-LAST:event_jButton1ActionPerformed

    private void repaintMap() {
        Graphics gr = canvas2.getGraphics();
        Graphics2D g = (Graphics2D) gr;
        g.setBackground(Color.black);
        g.clearRect(0, 0, canvas2.getWidth(), canvas2.getHeight());
        for (Star s : pcr.getStars()) {
            int x = (canvas2.getWidth() * s.getX_coord()) / 10000;
            int y = (canvas2.getHeight() * (10000 - s.getY_coord())) / 10000;
            g.setColor(pcr.getColor(s.getStarcolor()));
            int size = s.getStartype() + 3;
            if (starSelected != null) {
                if (starSelected.getName_constellation().equals(s.getName_constellation())) {
                    size += 3;
                }
            }

            g.fillOval(x, y, size, size);
        }

        for (Constellation c : pcr.getConstellations()) {
            for (ConstellationLine cl : c.cElements) {
                int x1 = (canvas2.getWidth() * cl.star1.getX_coord()) / 10000;
                int y1 = (canvas2.getHeight() * (10000 - cl.star1.getY_coord())) / 10000;
                int x2 = (canvas2.getWidth() * cl.star2.getX_coord()) / 10000;
                int y2 = (canvas2.getHeight() * (10000 - cl.star2.getY_coord())) / 10000;
                g.drawLine(x1, y1, x2, y2);
            }
        }

        updateStatusBar();
    }
    private Star starSelected = null;
    private ArrayList<Star> selectedStars = new ArrayList<Star>();

    private void canvas2MouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_canvas2MouseClicked
//mouseAction(evt);
    }//GEN-LAST:event_canvas2MouseClicked

    private void mouseAction(MouseEvent evt) {

        int mx = evt.getX();
        int my = evt.getY();

        int ox = (mx * 10000) / canvas2.getWidth();
        int oy = ((canvas2.getHeight() - my) * 10000) / canvas2.getHeight();

        HashMap<Star, Double> dists = new HashMap<Star, Double>();

        for (Star s : pcr.getStars()) {
            int sx = s.getX_coord();
            int sy = s.getY_coord();
            double csq = Math.pow(sx - ox, 2) + Math.pow(sy - oy, 2);
            dists.put(s, csq);
        }

        Star closestStar = null;
        Double closestDist = Double.MAX_VALUE;
        for (Star s : pcr.getStars()) {
            if (dists.get(s) < closestDist) {
                closestDist = dists.get(s);
                closestStar = s;
            }
        }

        starSelected = closestStar;
        System.out.println("Mousebtn " + evt.getButton() + " clicked at " + ox + "/" + oy + "  closest star = " + closestStar);



        if (evt.getButton() == evt.BUTTON1) {
            selectedStars.add(closestStar);
            while (selectedStars.size() >= 2) {
                ConstellationLine cl = new ConstellationLine(selectedStars.get(0), selectedStars.get(1));
                selectedStars.remove(cl.star1);
                selectedStars.remove(cl.star2);
                pcr.addConstellationLine(cl);
            }
        } else if (evt.getButton() == evt.BUTTON2) { //middlemouse, delete it
            for (Constellation c : pcr.getConstellations()) {
                if (c.constellationName.equals(starSelected.getName_constellation())) {
                    ArrayList<ConstellationLine> remLs = new ArrayList<ConstellationLine>();
                    for (ConstellationLine cl : c.cElements) {
                        if (cl.star1.name_prefix == starSelected.name_prefix || cl.star2.name_prefix == starSelected.name_prefix) {
                            remLs.add(cl);
                        }
                    }
                    for (ConstellationLine cl : remLs) {
                        c.cElements.remove(cl);
                    }
                }
            }
        } else if (evt.getButton() == evt.BUTTON3) { //rightmouse, do nothing
        }



        repaintMap();

        if (starSelected != null) {
            jLabel1.setText(starSelected.toMapToolString() + " // " + starSelected);
        }
    }

    private void canvas2MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_canvas2MousePressed
        mouseAction(evt);
    }//GEN-LAST:event_canvas2MousePressed

    private void canvas2MouseReleased(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_canvas2MouseReleased
        mouseAction(evt);
    }//GEN-LAST:event_canvas2MouseReleased

    private void jMenuItem5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem5ActionPerformed
        JFileChooser fc = new JFileChooser();
        if (fc.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = fc.getSelectedFile();
            try {
                pcr.writeXML(f);
            } catch (IOException ex) {
                Logger.getLogger(MapShow.class.getName()).log(Level.SEVERE, null, ex);
            }
            repaintMap();
        }
    }//GEN-LAST:event_jMenuItem5ActionPerformed

    private void jMenuItem6ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem6ActionPerformed
        JFileChooser fc = new JFileChooser();
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = fc.getSelectedFile();
            try {
                pcr.parseXML(f);
            } catch (IOException ex) {
                Logger.getLogger(MapShow.class.getName()).log(Level.SEVERE, null, ex);
            }
            repaintMap();
        }
    }//GEN-LAST:event_jMenuItem6ActionPerformed

    private void updateStatusBar() {
        jLabel2.setText(pcr.getstarnames().size() + " starnames loaded" + (!pcr.getstarnames().containsKey(Integer.MAX_VALUE) ? "" : " (BUILT IN P6014 STARNAMES)"));
        jLabel3.setText(pcr.getStars().size() + " Stars");
        int i = 0;
        for (Constellation c : pcr.getConstellations()) {
            for (ConstellationLine cl : c.cElements) {
                i++;
            }
        }
        jLabel4.setText(i + " Constellation Lines");
    }

    private void jMenuItem1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem1ActionPerformed
        if (pcr.getstarnames().containsKey(Integer.MAX_VALUE)) {
            JOptionPane.showMessageDialog(rootPane, "You didn't load a starnames text file. Now using built-in Project 6014 starnames (which MAY BE OUTDATED!). If you want to load a different set of starnames, please cancel and do so.");
        }
        JFileChooser fc = new JFileChooser();
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = fc.getSelectedFile();
            try {
                pcr.parsePlandata_C(f);
            } catch (IOException ex) {
                Logger.getLogger(MapShow.class.getName()).log(Level.SEVERE, null, ex);
            }
            repaintMap();
        }
    }//GEN-LAST:event_jMenuItem1ActionPerformed

    private void jMenuItem3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem3ActionPerformed
        StringBuilder sb = new StringBuilder();
        for (Constellation c : pcr.getConstellations()) {
            sb.append(c + "\n");
        }
        TextDialog td = new TextDialog(sb.toString(), "Constellations");
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem3ActionPerformed

    private void jMenuItem2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem2ActionPerformed
        StringBuilder sb = new StringBuilder();
        for (Star c : pcr.getStars()) {
            sb.append(c + "\n");
        }
        TextDialog td = new TextDialog(sb.toString(), "Stars");
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem2ActionPerformed

    private void jMenuItem4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem4ActionPerformed
        StringBuilder sb = new StringBuilder();
        for (Star c : pcr.getStars()) {
            sb.append(c.toMapToolString() + "\n");
        }
        TextDialog td = new TextDialog(sb.toString(), "Stars");
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem4ActionPerformed

    private void jPanel1ComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_jPanel1ComponentResized
        canvas2.setSize(jPanel1.getSize());
        repaintMap();
    }//GEN-LAST:event_jPanel1ComponentResized

    private void canvas2ComponentResized(java.awt.event.ComponentEvent evt) {//GEN-FIRST:event_canvas2ComponentResized
    }//GEN-LAST:event_canvas2ComponentResized

    private void jMenuItem7ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem7ActionPerformed
        JFileChooser fc = new JFileChooser();
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = fc.getSelectedFile();
            try {
                pcr.parseMaptoolFile(f);
            } catch (Exception ex) {
                TextDialog td = new TextDialog("" + ex, "Error occured whilst parsing");
                td.setVisible(true);
            }
            repaintMap();
        }
    }//GEN-LAST:event_jMenuItem7ActionPerformed

    private void jMenuItem8ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem8ActionPerformed
        JFileChooser fc = new JFileChooser();
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = fc.getSelectedFile();
            try {
                pcr.loadConstellationNames(f);
            } catch (Exception ex) {
                TextDialog td = new TextDialog("" + ex, "Error occured whilst parsing");
            }
            repaintMap();
        }
    }//GEN-LAST:event_jMenuItem8ActionPerformed

    private void jMenuItem9ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem9ActionPerformed
        StringBuilder sb = new StringBuilder();
        for (Integer i : pcr.getstarnames().keySet()) {
            if (i != Integer.MAX_VALUE) {
                String sname = pcr.getstarnames().get(i);
                int j = 0;
                for (Star s : pcr.getStars()) {
                    if (s.getName_constellation().equals(sname)) {
                        j++;
                    }
                }
                if (j == 0) {
                    System.out.println("Constellation not used: " + sname);
                }
                sb.append(i + ": " + sname + " [" + j + "]\n");
            }
        }
        TextDialog td = new TextDialog(sb.toString(), "Starnames");
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem9ActionPerformed

    private void jMenuItem10ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem10ActionPerformed
        TextDialog td = new TextDialog("MapConstellationEditor is a basic and simple 'constellation editor', to allow drawing of constellation lines easier.\n"
                + "\n"
                + "WARNING:\n"
                + "This tool was made for myself, to quickly draw constellation lines and generate output compatible with the 'map' tool.\n"
                + "As such, you should expect the following stuff:\n"
                + "-Bugs\n"
                + "-Annoying interface\n"
                + "-Shitty code that was written in a few seconds\n"
                + "-More bugs\n"
                + "-Features that are half-assed and only barely working\n"
                + "\n"
                + "You have been warned!!\n"
                + "\n"
                + "\n"
                + "TO OPERATE:\n"
                + "1) Load an existing .map file (it has to be a 'root' map file which references to stars and constellations, like \"large300-p6014.map\")\n"
                + "2) Rightclick somewhere in the main map to 'highlight' a constellation\n"
                + "3) Click and drag with the left mouse button to generate a constellation line (the closest star to the mouse coordinates is used, so you don't have to be precise)\n"
                + "4) Middleclick on a star to remove all constellation lines attached to it (in case you made a mistake)\n"
                + "5) In the menu: VIEW -> CONSTELLATIONS (MAPTOOL FORMAT). Copy&paste this in the .map file (\"clusters-p6014.map\")\n"
                + "Done!\n"
                + "\n"
                + "There are various other features that may be interesting (or not). Figuring out what they do is left as an exercise to the reader. :)", "Help");
        td.setWrapping(true, true);
        td.setVisible(true);
    }//GEN-LAST:event_jMenuItem10ActionPerformed

    private void jMenuItem11ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_jMenuItem11ActionPerformed
        JFileChooser fc = new JFileChooser();
        if (fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            File f = fc.getSelectedFile();
            try {
                DiaConverter.ConvertToDia(f, pcr.getStars(), pcr.getConstellations());
            } catch (Exception ex) {
                TextDialog td = new TextDialog("" + ex, "Error occured whilst saving DIA file");
            }
            repaintMap();
        }
    }//GEN-LAST:event_jMenuItem11ActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {

            public void run() {
                new MapShow().setVisible(true);
            }
        });
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private java.awt.Canvas canvas2;
    private javax.swing.JButton jButton1;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JMenu jMenu1;
    private javax.swing.JMenu jMenu2;
    private javax.swing.JMenu jMenu3;
    private javax.swing.JMenu jMenu4;
    private javax.swing.JMenu jMenu5;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JMenuItem jMenuItem1;
    private javax.swing.JMenuItem jMenuItem10;
    private javax.swing.JMenuItem jMenuItem11;
    private javax.swing.JMenuItem jMenuItem2;
    private javax.swing.JMenuItem jMenuItem3;
    private javax.swing.JMenuItem jMenuItem4;
    private javax.swing.JMenuItem jMenuItem5;
    private javax.swing.JMenuItem jMenuItem6;
    private javax.swing.JMenuItem jMenuItem7;
    private javax.swing.JMenuItem jMenuItem8;
    private javax.swing.JMenuItem jMenuItem9;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JToolBar.Separator jSeparator1;
    private javax.swing.JToolBar.Separator jSeparator2;
    private javax.swing.JPopupMenu.Separator jSeparator3;
    private javax.swing.JToolBar jToolBar1;
    // End of variables declaration//GEN-END:variables
}
