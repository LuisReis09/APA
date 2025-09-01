import { useCallback } from "react";
import { useDropzone } from "react-dropzone";
import { Card, CardContent } from "@/components/ui/card";
import { Upload, FileText, AlertCircle } from "lucide-react";
import { cn } from "@/lib/utils";

interface FileUploadProps {
  onFileUpload: (file: File) => void;
}

const FileUpload = ({ onFileUpload }: FileUploadProps) => {
  const onDrop = useCallback((acceptedFiles: File[]) => {
    if (acceptedFiles.length > 0) {
      onFileUpload(acceptedFiles[0]);
    }
  }, [onFileUpload]);

  const { getRootProps, getInputProps, isDragActive, isDragReject } = useDropzone({
    onDrop,
    accept: {
      'text/plain': ['.txt']
    },
    maxFiles: 1,
    multiple: false
  });

  return (
    <Card 
      {...getRootProps()} 
      className={cn(
        "cursor-pointer transition-all duration-200 border-2 border-dashed",
        isDragActive && !isDragReject && "border-primary bg-primary/5",
        isDragReject && "border-destructive bg-destructive/5",
        !isDragActive && "border-border hover:border-primary/50 hover:bg-primary/5"
      )}
    >
      <input {...getInputProps()} />
      <CardContent className="flex flex-col items-center justify-center py-8 text-center space-y-4">
        {isDragReject ? (
          <>
            <AlertCircle className="h-12 w-12 text-destructive" />
            <div>
              <p className="font-medium text-destructive">Arquivo não suportado</p>
              <p className="text-sm text-muted-foreground">Apenas arquivos .txt são aceitos</p>
            </div>
          </>
        ) : (
          <>
            <div className="p-3 bg-primary/10 rounded-full">
              {isDragActive ? (
                <Upload className="h-8 w-8 text-primary animate-bounce" />
              ) : (
                <FileText className="h-8 w-8 text-primary" />
              )}
            </div>
            <div>
              <p className="font-medium text-foreground">
                {isDragActive ? "Solte o arquivo aqui..." : "Clique ou arraste um arquivo"}
              </p>
              <p className="text-sm text-muted-foreground">
                Arquivo .txt contendo os dados do problema VRP
              </p>
            </div>
          </>
        )}
      </CardContent>
    </Card>
  );
};

export default FileUpload;